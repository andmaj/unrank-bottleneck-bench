/* sdsl - succinct data structures library
    Copyright (C) 2012 Simon Gog
*/

#include "sdsl/lcp_construct.hpp"

namespace sdsl
{

bool construct_lcp_kasai(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp", "construct LCP", "begin", 1, 0);
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    if (!construct_isa(file_map, dir, id)) {  // construct isa if it not already exists on disk
        return false;
    }
    {

        write_R_output("lcp", "load text", "begin", 1, 0);
        unsigned char* text = NULL;
        if (!util::load_from_int_vector_buffer(text, text_buf)) {
            throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
        }
        write_R_output("lcp", "load text", "end", 1, 0);


        int_vector_file_buffer<> isa_buf(file_map["isa"].c_str(), 1000000);   // init isa file_buffer

        int_vector<> sa;
        if (!util::load_from_file(sa, file_map["sa"].c_str())) {			   // init sa
            throw std::ios_base::failure("cst_construct: Cannot load SA from file system!");
        }
        // use Kasai algorithm to compute the lcp values
        for (size_type i=0,j=0,sa_1=0,l=0, r_sum=0, r=isa_buf.load_next_block(), n=isa_buf.int_vector_size; r_sum < n;) {
            for (; i < r_sum+r; ++i) {
                sa_1 =  isa_buf[i-r_sum]; // = isa[i]
                if (sa_1) {
                    j = sa[sa_1-1];
                    if (l) --l;
                    assert(i!=j);
                    while (text[i+l]==text[j+l]) { // i+l < n and j+l < n braucht man nicht, da text[n]=0 und text[i]!=0 (i<n) und i!=j
                        ++l;
                    }
                    sa[ sa_1-1 ] = l; //overwrite sa array with lcp values
                } else {
                    l = 0;
                    sa[ n-1 ] = 0;
                }
            }
            r_sum += r;
            r = isa_buf.load_next_block();
        }
        delete [] text; text = NULL;
        if (!util::store_to_file(sa, (dir+"lcp_"+id).c_str())) {
            throw std::ios_base::failure("cst_construct: Cannot store LCP to file system!");
            return false;
        } else {
            file_map["lcp"] = dir+"lcp_"+id;
        }
        // TODO: shift on disk, i.e. read from lcp_buf and write with a buffer to disk
        {
            int_vector_file_buffer<> lcp_buf(file_map["lcp"].c_str(), 1000000);
            // shift lcp array
            for (size_type i=0, r_sum=0, r=lcp_buf.load_next_block(); r_sum < sa.size();) {
                for (; i<r_sum+r-1; ++i) {
                    sa[i+1] = lcp_buf[i-r_sum];
                }
                r_sum += r;
                if (r_sum == sa.size())
                    sa[0] = lcp_buf[r-1];
                else
                    sa[i+1] = lcp_buf[r-1];
                ++i;
                r = lcp_buf.load_next_block();
            }
        } // destructor of lcp_buf is called
        if (!util::store_to_file(sa, file_map["lcp"].c_str())) {
            throw std::ios_base::failure("cst_construct: Cannot store ISA to file system!");
            return false;
        }
    }
    write_R_output("lcp", "construct LCP", "end", 1, 0);
    return true;
}

bool construct_lcp_semi_extern_PHI(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp", "construct LCP", "begin", 1, 0);
    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());
    size_type n = sa_buf.int_vector_size; // TODO: handle case n=0
    if (n==0) {
        file_map["lcp"] = dir+"lcp_"+id;
        int_vector<> lcp(0);
        util::store_to_file(lcp, file_map["lcp"].c_str());
        return true;
    }
    const uint8_t log_q = 6; // => q=64
    const uint32_t q = 1<<log_q;
    const uint64_t modq = bit_magic::Li1Mask[log_q];

    // n-1 is the gretes entry in SA
    int_vector<64> plcp((n-1+q)>>log_q);

    write_R_output("lcp", "calculate sparse phi", "begin", 1, 0);
    sa_buf.reset();
    for (size_type i=0, sai_1=0, r=0, r_sum=0; r_sum < n;) {   // we can start at i=0. if SA[i]%q==0
        for (; i < r_sum+r; ++i) {               // we set PHI[(SA[i]=n-1)%q]=0, since T[0]!=T[n-1]
            size_type sai = sa_buf[i-r_sum];
            if ((sai & modq) == 0) {
                if ((sai>>log_q) >= plcp.size()) {
                    std::cerr<<"sai="<<sai<<" log_q="<<log_q<<" sai>>log_q="<<(sai>>log_q)<<" "<<sai_1<<std::endl;
                    std::cerr<<"n="<<n<<" "<<" plcp.size()="<<plcp.size();
                }
                plcp[sai>>log_q] = sai_1;
            }
            sai_1 = sai;
        }
        r_sum += r; r = sa_buf.load_next_block();
    }
    write_R_output("lcp", "calculate sparse phi", "begin", 1, 0);

    write_R_output("lcp", "load text", "begin", 1, 0);
    unsigned char* text = NULL;
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    if (!util::load_from_int_vector_buffer(text, text_buf)) {
        throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
    }
    write_R_output("lcp", "load text", "end", 1, 0);


    write_R_output("lcp", "calculate sparse plcp", "begin", 1, 0);
    for (size_type i=0,j,k,l=0; i < plcp.size(); ++i) {
        j =	i<<log_q;   // j=i*q
        k = plcp[i];
        while (text[j+l] == text[k+l])
            ++l;
        plcp[i] = l;
        if (l >= q) {
            l -= q;
        } else {
            l = 0;
        }
    }
    write_R_output("lcp", "calculate sparse plcp", "end", 1, 0);


    file_map["lcp"] = dir+"lcp_"+id;
    std::ofstream lcp_out_buf(file_map["lcp"].c_str(), std::ios::binary | std::ios::app | std::ios::out);   // open buffer for plcp
    size_type bit_size = n*sa_buf.int_width;
    lcp_out_buf.write((char*) &(bit_size), sizeof(sa_buf.int_vector_size));		// write size of vector
    lcp_out_buf.write((char*) &(sa_buf.int_width), sizeof(sa_buf.int_width));       // write int_width of vector
    size_type wb = 0;

    size_type buffer_size = 4000000; // buffer_size is a multiple of 8!
    sa_buf.reset(buffer_size);
    for (size_type i=0, r=0, r_sum=0, sai_1=0,l=0, sai=0,iq=0; r_sum < n;) {
        for (; i < r_sum+r; ++i) {
            /*size_type*/ sai = sa_buf[i-r_sum];
//				std::cerr<<"i="<<i<<" sai="<<sai<<std::endl;
            if ((sai & modq) == 0) { // we have already worked the value out ;)
                sa_buf.set_int(i-r_sum, l=plcp[sai>>log_q]);
            } else {
                /*size_type*/ iq = sai & bit_magic::Li0Mask[log_q];
                l  = plcp[sai>>log_q];
                if (l >	(sai-iq))
                    l -= (sai-iq);
                else
                    l=0;
                while (text[ sai+l ] == text[ sai_1+l ])
                    ++l;
                sa_buf.set_int(i-r_sum, l);
            }
#ifdef CHECK_LCP
            size_type j=0;
            for (j=0; j<l; ++j) {
                if (text[sai+j] !=text[sai_1+j]) {
                    std::cout<<"lcp["<<i<<"]="<<l<<" is two big! "<<j<<" is right!"<<" sai="<<sai<<std::endl;
                    if ((sai&modq)!=0)
                        std::cout<<" plcp[sai>>log_q]="<<plcp[sai>>log_q]<<" sai-iq="<<sai-iq<<" sai="<<sai<<" sai-iq="<<sai-iq<<std::endl;
                    break;
                }
            }
#endif
            sai_1 = sai;
        }
        if (r > 0) {
            size_type cur_wb = (r*sa_buf.int_width+7)/8;
            lcp_out_buf.write((const char*)sa_buf.data(), cur_wb);
            wb += cur_wb;
        }
        r_sum += r; r = sa_buf.load_next_block();
    }

    if (wb%8) {
        lcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
    }
    lcp_out_buf.close();
    delete [] text;
    write_R_output("lcp", "construct LCP", "end", 1, 0);
    return true;
}

bool construct_lcp_PHI(tMSS& file_map, const std::string& dir, const std::string& id, bool semi_external)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp", "construct LCP", "begin", 1, 0);
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());

    size_type n = sa_buf.int_vector_size; // TODO: handle case n=0
    int_vector<> plcp(n, 0, sa_buf.int_width);
    for (size_type i=0, r_sum=0, r=sa_buf.load_next_block(), sai_1 = 0; r_sum < n;) {
        for (; i < r_sum+r; ++i) {
            size_type sai = sa_buf[i-r_sum];
            plcp[ sai ] = sai_1;
            sai_1 = sai;
        }
        r_sum += r; r = sa_buf.load_next_block();
    }

    std::string file_name_PHI = dir+"PHI_"+id;
    std::string file_name_PLCP = dir+"plcp_"+id;
    if (semi_external) {
        // store PHI (content of plcp) to disk
        util::store_to_file(plcp, file_name_PHI.c_str());
        plcp.resize(0); // resize plcp to 0
    }

    write_R_output("lcp", "load text", "begin", 1, 0);
    unsigned char* text = NULL;
    if (!util::load_from_int_vector_buffer(text, text_buf)) { // load text
        throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
    }
    write_R_output("lcp", "load text", "end", 1, 0);

    if (!semi_external) {
        for (size_type i=0, l=0; i < n-1; ++i) {
            size_type phii = plcp[i];
            while (text[i+l] == text[phii+l]) {
                ++l;
            }
            plcp[i] = l;
            if (l)
                --l;
        }
    } else { // external version is about 20 % slower than normal version
        std::ofstream plcp_out_buf(file_name_PLCP.c_str(), std::ios::binary | std::ios::app | std::ios::out);   // open buffer for plcp

        size_type bit_size = n*sa_buf.int_width;
        plcp_out_buf.write((char*) &(bit_size), sizeof(sa_buf.int_vector_size));		// write size of vector
        plcp_out_buf.write((char*) &(sa_buf.int_width), sizeof(sa_buf.int_width));       // write int_width of vector
        size_type wb = 0;

        size_type buffer_size = 1000000; // buffer_size is a multiple of 8!
        int_vector_file_buffer<> phi_buf(file_name_PHI.c_str(), buffer_size);
//			int_vector<> plcp_buf(buffer_size, 0, phi_buf.int_width );
//			size_type phii_1 = phi_buf[0];
        for (size_type i=0, l=0, r=0, r_sum=0; r_sum < n-1;) { // TODO: case plcp[n-1]?
            for (; i < r_sum + r ; ++i) {
                size_type phii = phi_buf[i-r_sum];
//					if(phii != phii_1+1){
                while (text[i+l] == text[phii+l])
                    ++l;
//					}
//					plcp_buf[i-r_sum] = l; // TODO: change int_vector_file_buffer so that we can assign phi_buf the new value
                phi_buf.set_int(i-r_sum, l);
                if (l)
                    --l;
//					phii_1 = phii;
            }
            if (r > 0) {
                size_type cur_wb = (r*phi_buf.int_width+7)/8;
//					plcp_out_buf.write((const char*)plcp_buf.data(), cur_wb );
                plcp_out_buf.write((const char*)phi_buf.data(), cur_wb);
                wb += cur_wb;
            }
            r_sum += r; r = phi_buf.load_next_block();
        }

        if (wb%8) {
            plcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
        }
        plcp_out_buf.close();
    }

    delete [] text;

    if (semi_external) {
        util::load_from_file(plcp, file_name_PLCP.c_str());   // load PLCP from disk
        std::remove(file_name_PHI.c_str());
        std::remove(file_name_PLCP.c_str());
    }

    std::ofstream lcp_out_buf((dir+"lcp_"+id).c_str(), std::ios::binary | std::ios::app | std::ios::out);   // open buffer for lcp
    file_map["lcp"] = dir+"lcp_"+id;

    size_type bit_size = n*sa_buf.int_width;
    lcp_out_buf.write((char*) &(bit_size), sizeof(sa_buf.int_vector_size));		// write size of vector
    lcp_out_buf.write((char*) &(sa_buf.int_width), sizeof(sa_buf.int_width));       // write int_width of vector
    size_type wb = 0;

    size_type buffer_size = 1000000; // buffer_size is a multiple of 8!

    int_vector<> lcp_buf(buffer_size, 0, sa_buf.int_width);
    lcp_buf[0] = 0;
    sa_buf.reset(buffer_size);
    size_type r = 0;// sa_buf.load_next_block();
    for (size_type i=1, r_sum=0; r_sum < n;) {
        for (; i < r_sum+r; ++i) {
            size_type sai = sa_buf[i-r_sum];
            lcp_buf[ i-r_sum ] = plcp[sai];
        }
        if (r > 0) {
            size_type cur_wb = (r*lcp_buf.get_int_width()+7)/8;
            lcp_out_buf.write((const char*)lcp_buf.data(), cur_wb);
            wb += cur_wb;
        }
        r_sum += r; r = sa_buf.load_next_block();
    }
    if (wb%8) {
        lcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
    }
    lcp_out_buf.close();

    write_R_output("lcp", "construct LCP", "end", 1, 0);
    return true;
}

buffered_char_queue::buffered_char_queue():m_widx(0), m_ridx(0), m_sync(true), m_disk_buffered_blocks(0), m_c('?'),m_rb(0), m_wb(0) {};

void buffered_char_queue::init(const std::string& dir, char c)
{
    m_c = c;
    m_file_name = dir+"buffered_char_queue_"+util::to_string(util::get_id());
//		m_stream.rdbuf()->pubsetbuf(0, 0);
}

buffered_char_queue::~buffered_char_queue()
{
    m_stream.close();
    std::remove(m_file_name.c_str());
}

void buffered_char_queue::push_back(uint8_t x)
{
    m_write_buf[m_widx] = x;
    if (m_sync) {
        m_read_buf[m_widx] = x;
    }
    ++m_widx;
    if (m_widx == m_buffer_size) {
        if (!m_sync) { // if not sync, write block to disk
            if (!m_stream.is_open()) {
                m_stream.open(m_file_name.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
            }
            m_stream.seekp(m_buffer_size * (m_wb++), std::ios::beg);
//				std::cout<<"tellg="<<m_stream.tellg()<<" tellp"<<m_stream.tellp()<<std::endl;
            m_stream.write((char*) m_write_buf, m_buffer_size);
//				std::cout<<"tellg="<<m_stream.tellg()<<" tellp"<<m_stream.tellp()<<std::endl;
            ++m_disk_buffered_blocks;
        }
        m_sync = 0;
        m_widx = 0;
    }
}

uint8_t buffered_char_queue::pop_front()
{
    uint8_t x = m_read_buf[m_ridx];
    ++m_ridx;
    if (m_ridx ==  m_buffer_size) {
        if (m_disk_buffered_blocks > 0) {
            m_stream.seekg(m_buffer_size * (m_rb++), std::ios::beg);
            m_stream.read((char*) m_read_buf, m_buffer_size);
            --m_disk_buffered_blocks;
        } else { // m_disk_buffered_blocks == 0
            m_sync = 1;
            memcpy(m_read_buf, m_write_buf, m_widx+1);
        }
        m_ridx = 0;
    }
    return x;
}

bool construct_lcp_simple_5n(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp","construct LCP", "begin", 1, 0);
    construct_bwt(file_map, dir, id);

    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());   // initialize buffer for suffix array
    sa_buf.load_next_block();
    size_type sai_1 = sa_buf[0];  // store value of sa[i-1]
    int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // initialize buffer of bwt
    size_type r = bwt_buf.load_next_block();
    uint8_t bwti_1 = bwt_buf[0];       // store value of BWT[i-1]
    int_vector<8> text;
    util::load_from_file(text, file_map["text"].c_str());

    const size_type n = sa_buf.int_vector_size;

    size_type cnt_c[257] = {0}; // counter for each character in the text
    size_type cnt_cc[257] = {0};  // prefix sum of the counter cnt_c
    size_type prev_occ_in_bwt[256] = {0};  // position of the previous occurence of each character c in the bwt
    for (size_type i=0; i<n; ++i) { // initialize cnt_c
        ++cnt_c[text[i]+1];
    }
    unsigned char alphabet[257] = {0};
    uint8_t sigma = 0;
    for (int i=1; i<257; ++i) { // calculate sigma and initailize cnt_cc
        if (cnt_c[i] > 0) {
            alphabet[sigma++] = (unsigned char)(i-1);
        }
        cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
    }
    for (size_type i=0; i<256; ++i) prev_occ_in_bwt[i] = (size_type)-1; // initialze the array with -1

    int_vector<> lcp(n, 0, sa_buf.int_width);
    lcp[ cnt_cc[bwti_1]++ ] = 0;   // lcp[ LF[0] ] = 0

    const size_type update_stack = 1024;

    int_vector<64> rmq_stack(2*(update_stack + sigma + 8));   // initialize stack for (update_stack+sigma+8) elements representing (position, value)
    rmq_stack[0] = 0; rmq_stack[1] = 0;  // first element (-1, -1)
    rmq_stack[2] = 1; rmq_stack[3] = 0;  // second element (0, -1)
    size_type rmq_end=3;				 // index of the value of the topmost element
    const size_type rmq_limit = rmq_stack.size()-4;
    uint8_t cur_c = alphabet[1];
    size_type comps = 0;

    size_type queries[257] = {0};
    for (size_type i=1, sai, r_sum=0, cur_c_idx=1, cur_c_cnt=cnt_c[alphabet[1]+1]; r_sum < n;) {
        for (; i < r_sum+r; ++i, --cur_c_cnt) {
            uint8_t bwti = bwt_buf[i-r_sum];
            sai = sa_buf[i-r_sum];
            size_type lf = cnt_cc[bwti];
            if (!cur_c_cnt) {// cur_c_cnt==0, if there is no more occurence of the current character
                if (cur_c_cnt < sigma) {
                    cur_c_cnt = cnt_c[(cur_c=alphabet[++cur_c_idx])+1];
                }
            }
            size_type l=0;
            if (i >= cnt_cc[cur_c]) { // if the current lcp entry is not already done
                if (lf < i) {
                    l = lcp[lf] ? lcp[lf]-1 : 0; // l = LCP[LF[i]]-1; l < m+1
                    if (bwti == bwti_1)
                        goto calculated_l;
                }
                while (text[sai_1+l] == text[sai+l]) {
                    ++l;
                    ++comps;
                }
//					++comps;
calculated_l:
                lcp[i] = l;
            } else { // if already done
                l = lcp[i];  // load LCP value
            }
            // begin update rmq_stack
            size_type x = l+1;
            size_type j = rmq_end;
            while (x <= rmq_stack[j]) j-=2;  // pop all elements with value >= l
            rmq_stack[++j] = i+1; // push position i
            rmq_stack[++j] = x;	  // push value	l
            rmq_end = j;          // update index of the value of the topmost element
            if (lf > i) {   // if LF[i] > i, we can calculate LCP[LF[i]] in constant time with rmq
                // rmq query for lcp-values in the interval I=[prev_occ_in_bwt[BWT[i]]+1..i]
                // rmq is linear in the stack size; can also be implemented with binary search on the stack
                size_type x_pos = prev_occ_in_bwt[bwti]+2;
                size_type j = rmq_end-3;
                while (x_pos <= rmq_stack[j]) j-=2;   //  search smallest value in the interval I
                lcp[lf] = rmq_stack[j+3];
            }
            prev_occ_in_bwt[bwti] = i;  	 // update previous position information for character BWT[i]
            ++cnt_cc[bwti];					 // update counter and therefore the LF information
            sai_1 = sai;					 // update SA[i-1]
            bwti_1 = bwti;					 // update BWT[i-1]
            if (rmq_end > rmq_limit) {
//					std::cout<<"stack is too big (i="<<i<<"). Stack size = "<< (rmq_end+1)/2 <<". Start resizing."<<std::endl;
                for (int j=0; j<sigma; ++j)
                    queries[j] = prev_occ_in_bwt[alphabet[j]];
                std::sort(queries, queries+sigma);
                size_type new_rmq_end = 3;
                for (int k=0, j=4; k<sigma; ++k) {
                    size_type x_pos = queries[k]+2;
                    if (rmq_stack[new_rmq_end-1] < x_pos) { // otherwise we need the previous stack element to answer the query
                        while (x_pos < rmq_stack[j]) {
                            j += 2;
                            assert(j < rmq_end);
                        }
                        // now x_pos >= rmq_stack[jj]
                        rmq_stack[++new_rmq_end] = rmq_stack[j];
                        rmq_stack[++new_rmq_end] = rmq_stack[j+1];
                    }
                }
//					std::cout<<"rmq_end = "<<rmq_end<<" new_rmq_end = "<<new_rmq_end<<std::endl;
                rmq_end = new_rmq_end;
            }
        }
        r_sum += r; r = bwt_buf.load_next_block();
        sa_buf.load_next_block();
    }
    std::cout<<"# comparisons: "<<comps<<std::endl;
    if (!util::store_to_file(lcp, (dir+"lcp_"+id).c_str())) {  // store the LCP values
        throw std::ios_base::failure("cst_construct: Cannot store LCP_sml to file system!");
        return false;
    } else {
        file_map["lcp"] = dir+"lcp_"+id;
    };
    write_R_output("lcp","construct LCP", "end", 1, 0);
    return true;
}

bool construct_lcp_simple2_9n(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp","construct LCP", "begin", 1, 0);
    construct_bwt(file_map, dir, id);
    int_vector<8> bwt;
    util::load_from_file(bwt, file_map["bwt"].c_str());
    const size_type n = bwt.size();
    int_vector<> lcp(n, n-1, bit_magic::l1BP(n-1)+1);
    int_vector<> lf(n, 0, bit_magic::l1BP(n-1)+1);

    size_type cnt_c[257] = {0};   // counter for each character in the text
    size_type cnt_cc[257] = {0};  // prefix sum of the counter cnt_c
    size_type cnt_ccc[257] = {0};  // copy of the prefix sum of the counter cnt_c
    for (size_type i=0; i<n; ++i) { // initialize cnt_c
        ++cnt_c[bwt[i]+1];
    }
    for (int i=1; i<257; ++i) { // calculate sigma and initailize cnt_cc
        cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
        cnt_ccc[i] = cnt_cc[i];
    }
    for (size_type i=0; i<n; ++i) {
        lf[i] = cnt_ccc[bwt[i]];
        ++cnt_ccc[bwt[i]];
    }

    tVI m_indices[2];
    size_type m = 0;
    lcp[0] = 0;
    for (int i=1; i<257; ++i) {
//			for(size_type j=cnt_cc[i-1]+1; j < cnt_cc[i]; ++j){
//				lcp[j] = n-1;
//			}
        if (cnt_cc[i-1] != cnt_cc[i] and i<257) {
            m_indices[m].push_back(cnt_cc[i]);
        }
    }

//		tLI &cur_list = m_indices[m];
//		tLI &next_list = m_indices[(m+1)%2];
    size_type up_char_ex[257] = {0};
    while (!m_indices[m%2].empty()) {
//			std::cerr<<"m="<<m<<" list.size()="<<m_indices[m%2].size()<<std::endl;
        for (size_type j=0; j< m_indices[m%2].size(); ++j) {
            size_type i = m_indices[m%2][j];
            size_type no_of_chars=0;
            size_type k=i;
            while (k > 0 and lcp[k] >= m) {
                if (up_char_ex[bwt[k-1]] != i) {
                    up_char_ex[bwt[k-1]] = i;
                    ++no_of_chars;
                }
                --k;
            }
            k=i;
            while (no_of_chars > 0 and k < n and lcp[k]>m) {
                if (up_char_ex[bwt[k]] == i) {
                    m_indices[(m+1)%2].push_back(lf[k]);
                    up_char_ex[bwt[k]] = 0;
                    --no_of_chars;
                }
                ++k;
            }
            lcp[i] = m;
        }
        m_indices[m%2].clear();
        ++m;
    }

    if (!util::store_to_file(lcp, (dir+"lcp_"+id).c_str())) {  // store the LCP values
        throw std::ios_base::failure("cst_construct: Cannot store LCP_sml to file system!");
        return false;
    } else {
        file_map["lcp"] = dir+"lcp_"+id;
    };
    write_R_output("lcp","construct LCP", "end", 1, 0);
    return true;
}

bool construct_lcp_go(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
#ifdef STUDY_INFORMATIONS
    size_type racs  = 0; // random accesses to the text
    size_type matches = 0;
    size_type comps2 = 0; // comparisons the second phase
#endif
    write_R_output("lcp","construct LCP", "begin", 1, 0);
    construct_bwt(file_map, dir, id);
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    unsigned char* text = NULL;
    if (!util::load_from_int_vector_buffer(text, text_buf)) {  // load text from file system
        throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
    }
    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());   // initialize buffer for suffix array
    const size_type n = sa_buf.int_vector_size;
    const size_type m = 254; // LCP[i] == m+1 corresp. to LCP[i]>= m+1; LCP[i] <= m corresp. to LCP[i] was calculated

    if (n == 0) // if the size of SA is 0 we are already done
        return true;

    size_type cnt_c[257] = {0};   // counter for each character in the text
    size_type cnt_cc[257] = {0};  // prefix sum of the counter cnt_c
    size_type cnt_cc2[257] = {0};  //
    size_type omitted_c[257] = {0};  // counts the omitted occurences for the second phase
    size_type prev_occ_in_bwt[256] = {0};  // position of the previous occurence of each character c in the bwt
    for (size_type i=0; i<256; ++i) prev_occ_in_bwt[i] = (size_type)-1; // initialze the array with -1
    unsigned char alphabet[257] = {0};
    uint8_t sigma = 0;

    tLI m_list[2][256];
    size_type m_char_count[2] = {0};
    uint8_t m_chars[2][256] = {{0},{0}};

    size_type nn = 0; // n' for phase 2
    // phase 1: calculate lcp_sml;  memory consumption: 2n bytes (lcp_sml=n bytes, text=n bytes)
    {

        int_vector<8> lcp_sml(n, 0); // initialize array for small values of first phase; note lcp[0]=0
        size_type done_cnt=0;

        for (size_type i=0; i<n; ++i) { // initialize cnt_c
            ++cnt_c[text[i]+1];
        }
        for (int i=1; i<257; ++i) { // calculate sigma and initailize cnt_cc
            if (cnt_c[i] > 0) {
                alphabet[sigma++] = (unsigned char)(i-1);
            }
            cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
        }
        alphabet[sigma] = '\0';
        {
            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // initialize buffer of bwt
            sa_buf.reset();
            sa_buf.load_next_block();
            size_type sai_1 = sa_buf[0];  // store value of sa[i-1]
            size_type r = bwt_buf.load_next_block();
            uint8_t bwti_1 = bwt_buf[0];       // store value of BWT[i-1]
            lcp_sml[ cnt_cc[bwti_1]++ ] = 0;   // lcp_sml[ LF[0] ] = 0
            prev_occ_in_bwt[bwti_1] = 0;  // init previous occurence of character BWT[0]
            ++omitted_c[alphabet[0]];	  //

            int_vector<64> rmq_stack(2*(m+10));   // initialize stack for m+10 elements representing (position, value)
            rmq_stack[0] = 0; rmq_stack[1] = 0;  // first element (-1, -1)
            rmq_stack[2] = 1; rmq_stack[3] = 0;  // second element (0, -1)
            size_type rmq_end=3;				 // index of the value of the topmost element

            const size_type m_mod2 = m%2;
            uint8_t cur_c = alphabet[1];
            size_type big_val = 0;
            for (size_type i=1, sai, r_sum=0, cur_c_idx=1, cur_c_cnt=cnt_c[alphabet[1]+1]; r_sum < n;) {
                for (; i < r_sum+r; ++i, --cur_c_cnt) {
                    uint8_t bwti = bwt_buf[i-r_sum];
                    sai = sa_buf[i-r_sum];
                    size_type lf = cnt_cc[bwti];
                    if (!cur_c_cnt) {// cur_c_cnt==0, if there is no more occurence of the current character
                        if (cur_c_cnt < sigma) {
                            cur_c_cnt = cnt_c[(cur_c=alphabet[++cur_c_idx])+1];
                        }
                    }
                    size_type l=0;
                    if (i >= cnt_cc[cur_c]) { // if the current lcp entry is not already done   TODO: schleife von i bis cnt_cc[cur_c]
                        if (bwti == bwti_1 and lf < i) {  // BWT[i]==BWT[i-1]
                            l = lcp_sml[lf] ? lcp_sml[lf]-1 : 0; // l = LCP[LF[i]]-1; l < m+1
                            if (l == m) { // if LCP[LF[i]] == m+1; otherwise LCP[LF[i]] < m+1  the result is correct
                                l += (text[sai_1+m] == text[sai+m]);
#ifdef STUDY_INFORMATIONS
                                if ((sai_1^sai)>>6) // if i and phii are in the same cache line
                                    ++racs;
#endif
                            }
                            lcp_sml[i] = l;
                            ++done_cnt;
                        } else { // BWT[i] != BWT[i-1] or LF[i] > i
                            if (lf < i)
                                l = lcp_sml[lf] ? lcp_sml[lf]-1 : 0;
#ifdef STUDY_INFORMATIONS
                            if ((sai_1^sai)>>6) // if i and phii are in the same cache line
                                ++racs;
#endif
                            while (text[sai_1+l] == text[sai+l] and l < m+1) {
                                ++l;
#ifdef STUDY_INFORMATIONS
                                ++matches;
#endif
                            }
                            lcp_sml[i] = l;
                        }
                    } else { // if already done
                        l = lcp_sml[i];  // load LCP value
                    }
                    if (l > m) {
                        ++big_val;
                        if (i > 10000 and i < 10500 and big_val > 3000) { // if most of the values are big: switch to PHI algorithm
                            delete [] text; text = NULL;
                            lcp_sml.resize(0);
                            bool res = construct_lcp_PHI(file_map, dir, id);
                            write_R_output("lcp","construct LCP", "end", 1, 0);
                            return res;
                        }
                    }
                    // invariant: l <= m+1
                    // begin update rmq_stack
                    size_type x = l+1;
                    size_type j = rmq_end;
                    while (x <= rmq_stack[j]) j-=2;  // pop all elements with value >= l
                    rmq_stack[++j] = i+1; // push position i
                    rmq_stack[++j] = x;	  // push value	l
                    rmq_end = j;          // update index of the value of the topmost element
                    if (lf > i) {   // if LF[i] > i, we can calculate LCP[LF[i]] in constant time with rmq
                        ++done_cnt;
                        // rmq query for lcp-values in the interval I=[prev_occ_in_bwt[BWT[i]]+1..i]
                        // rmq is linear in the stack size; can also be implemented with binary search on the stack
                        size_type x_pos = prev_occ_in_bwt[bwti]+2;
                        size_type j = rmq_end-3;
                        while (x_pos <= rmq_stack[j]) j-=2;   //  search smallest value in the interval I
                        lcp_sml[lf] = rmq_stack[j+3] - (rmq_stack[j+3]==m+2); // if lcp-value equals m+1, we subtract 1
                    }
                    if (l >= m) {
                        if (l == m)
                            push_front_m_index(nn, cur_c, m_list[m_mod2], m_chars[m_mod2], m_char_count[m_mod2]);
                        ++nn;
                    } else
                        ++omitted_c[cur_c];

                    prev_occ_in_bwt[bwti] = i;  	 // update previous position information for character BWT[i]
                    ++cnt_cc[bwti];					 // update counter and therefore the LF information
                    sai_1 = sai;					 // update SA[i-1]
                    bwti_1 = bwti;					 // update BWT[i-1]
                }
                r_sum += r; r = bwt_buf.load_next_block();
                sa_buf.load_next_block();
            }
        }
        delete [] text; text = NULL;

        if (n > 1000 and nn > 5*(n/6)) {  // if we would occupy more space than the PHI algorithm => switch to PHI algorithm
            lcp_sml.resize(0);
            bool res = construct_lcp_PHI(file_map, dir, id);
            write_R_output("lcp","construct LCP", "end", 1, 0);
            return res;
        }

        if (!util::store_to_file(lcp_sml, (dir+"lcp_sml_"+id).c_str())) {  // store the small LCP values
            throw std::ios_base::failure("cst_construct: Cannot store LCP_sml to file system!");
            return false;
        } else {
            file_map["lcp_sml"] = dir+"lcp_sml_"+id;
        };
    }
#ifdef STUDY_INFORMATIONS
    std::cout<<"# n="<<n<<" nn="<<nn<<" nn/n="<<((double)nn)/n<<std::endl;
#endif

    // phase 2: calculate lcp_big
    {
//			std::cout<<"# begin calculating LF' values"<<std::endl;
        int_vector<> lcp_big(nn, 0, bit_magic::l1BP(n-1)+1); // lcp_big first contains adapted LF values and finally the big LCP values
        {
            // initialize lcp_big with adapted LF values
            bit_vector todo(n,0);  // bit_vector todo indicates which values are >= m in lcp_sml
            {
                // initialize bit_vector todo
                int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
                for (size_type i=0, r_sum=0, r = lcp_sml_buf.load_next_block(); r_sum < n;) {
                    for (; i < r_sum+r; ++i) {
                        if (lcp_sml_buf[i-r_sum] >= m) {
                            todo[i] = 1;
                        }
                    }
                    r_sum += r; r = lcp_sml_buf.load_next_block();
                }
            }

            cnt_cc2[0] = cnt_cc[0]= 0;
            for (size_type i=1, omitted_sum=0; i<257; ++i) { // initialize cnt_cc and cnt_cc2
                cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
                omitted_sum += omitted_c[i-1];
                cnt_cc2[i] = cnt_cc[i] - omitted_sum;
            }

            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // load BWT
            for (size_type i=0, i2=0, r_sum=0, r=bwt_buf.load_next_block(); r_sum < n;) {
                for (; i < r_sum+r; ++i) {
                    uint8_t b = bwt_buf[i - r_sum];  // store BWT[i]
                    size_type lf_i = cnt_cc[b]; // LF[i]
                    if (todo[i]) { // LCP[i] is a big value
                        if (todo[lf_i]) { // LCP[LF[i]] is a big entry
                            lcp_big[i2] = cnt_cc2[b]; // LF'[i]
                        }/*else{
								lcp_big[i2] = 0;
							}*/
                        ++i2;
                    }
                    if (todo[lf_i]) { // LCP[LF[i]] is a big entry
                        ++cnt_cc2[b];	// increment counter for adapted LF
                    }
                    ++cnt_cc[b]; // increment counter for LF
                }
                r_sum += r; r = bwt_buf.load_next_block();
            }
        }

//			std::cout<<"# begin initializing bwt2, shift_bwt2, run2"<<std::endl;
        int_vector<8> bwt2(nn), shift_bwt2(nn); // BWT of big LCP values, and shifted BWT of big LCP values
        bit_vector run2(nn+1);					// indicates for each entry i, if i and i-1 are both big LCP values
        run2[nn] = 0;							// index nn is not a big LCP value
        {
            // initialize bwt2, shift_bwt2, adj2
            int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // load BWT
            uint8_t b_1 = '\0'; // BWT[i-1]
            bool is_run = false;
            for (size_type i=0, i2=0, r_sum=0, r = 0; r_sum < n;) {
                for (; i < r_sum+r; ++i) {
                    uint8_t b = bwt_buf[i-r_sum];
                    if (lcp_sml_buf[i-r_sum] >= m) {
                        bwt2[i2] 		= b;
                        shift_bwt2[i2]	= b_1;
                        run2[i2] 		= is_run;
                        is_run 			= true;
                        ++i2;
                    } else {
                        is_run = false;
                    }
                    b_1 = b;
                }
                r_sum += r; r = lcp_sml_buf.load_next_block();
                bwt_buf.load_next_block();
            }
        }

        bit_vector todo2(nn+1, 1); // init all values with 1, except
        todo2[nn] = 0; 			   // the last one! (handels case "i < nn")

//			std::cout<<"# begin calculating m-indices"<<std::endl;
        {
            // calculate m-indices, (m+1)-indices,... until we are done
            size_type m2 = m;
            size_type char_ex[256]; for (size_type i=0; i<256; ++i) char_ex[i] = nn;
            size_type char_occ=0;
            size_type m_mod2 = m2%2, mm1_mod2 = (m2+1)%2;
            while (m_char_count[m_mod2] > 0) { // while there are m-indices, calculate (m+1)-indices and write m-indices
                // For all values LCP[i] >= m2 it follows that todo2[i] == 1
                // list m_list[mm1_mod2][b] is sorted in decreasing order
                ++m2;
                mm1_mod2 = (m2+1)%2, m_mod2 = m2%2;
                m_char_count[m_mod2] = 0;

                std::sort(m_chars[mm1_mod2], m_chars[mm1_mod2]+m_char_count[mm1_mod2]); // TODO: ersetzen?

                for (size_type mc=0; mc<m_char_count[mm1_mod2]; ++mc) { // for every character
                    tLI& mm1_mc_list = m_list[mm1_mod2][m_chars[mm1_mod2][ m_char_count[mm1_mod2]-1-  mc ]];
//						size_type old_i = nn;
                    while (!mm1_mc_list.empty()) {
                        size_type i = mm1_mc_list.front();  // i in [0..n-1]
                        mm1_mc_list.pop_front();
                        // For all values LCP[i] >= m-1 it follows that todo2[i] == 1
                        for (size_type k=i; todo2[k]; --k) {
#ifdef STUDY_INFORMATIONS
                            ++comps2;
#endif
                            uint8_t b = shift_bwt2[k];
                            if (char_ex[b] != i) {
                                char_ex[b] = i;
                                ++char_occ;
                            }
                            if (!run2[k])
                                break;
                        }
                        for (size_type k=i; todo2[k] and char_occ; ++k) {
#ifdef STUDY_INFORMATIONS
                            ++comps2;
#endif
                            uint8_t b = bwt2[k];
                            if (char_ex[b] == i) {
                                size_type p = lcp_big[k];
                                push_back_m_index(p, b, m_list[m_mod2], m_chars[m_mod2], m_char_count[m_mod2]);
                                char_ex[b] = nn;
                                --char_occ;
                            }
                            if (!run2[k+1])
                                break;
                        }
                        lcp_big[ i ] = m2-1;
                        todo2[ i ] = 0;
//							old_i = i;
                    }
                }
            }

        }

        if (!util::store_to_file(lcp_big, (dir+"lcp_big_"+id).c_str())) {  // store the big LCP values
            throw std::ios_base::failure("cst_construct: Cannot store LCP_big to file system!");
            return false;
        } else {
            file_map["lcp_big"] = dir+"lcp_big_"+id;
        };
    } // end phase 2

//		std::cout<<"# merge lcp_sml and lcp_big"<<std::endl;
    // phase 3: merge lcp_sml and lcp_big and save to disk
    {
        const size_type buffer_size = 1000000; // buffer_size has to be a multiple of 8!
        int_vector_file_buffer<> lcp_big_buf(file_map["lcp_big"].c_str()); 				// file buffer containing the big LCP values
        int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str(), buffer_size);// file buffer containing the small LCP values

        std::ofstream lcp_out_buf((dir+"lcp_"+id).c_str(), std::ios::binary | std::ios::trunc | std::ios::out);    // open out file stream
        file_map["lcp"] = dir+"lcp_"+id;  																		   // and save result to disk
        int_vector<> lcp_buf(buffer_size, 0, lcp_big_buf.int_width); // buffer for the resulting LCP array
        size_type bit_size = n*lcp_big_buf.int_width;
        lcp_out_buf.write((char*) &(bit_size), sizeof(lcp_big_buf.int_vector_size));		   // write size
        lcp_out_buf.write((char*) &(lcp_big_buf.int_width), sizeof(lcp_big_buf.int_width));   // write int_width
        size_type wb = 0; // written bytes of data

        for (size_type i=0, r_sum=0, r=0, i2=0 ,r2_sum=0, r2=0; r_sum < n;) {
            for (; i < r_sum + r; ++i) {
                size_type l = lcp_sml_buf[i-r_sum];
                if (l >= m) { // if l >= m it is stored in lcp_big
                    if (i2 >= r2_sum + r2) {
                        r2_sum += r2; r2 = lcp_big_buf.load_next_block();
                    }
                    l = lcp_big_buf[i2-r2_sum];
                    ++i2;
                }
                lcp_buf[i-r_sum] = l;
            }
            if (r > 0) {
                size_type cur_wb = r*lcp_buf.get_int_width();
                if (cur_wb%8 != 0)
                    cur_wb += 7;
                cur_wb /= 8;
                lcp_out_buf.write((const char*)lcp_buf.data(), cur_wb);
                wb += cur_wb;
            }
            r_sum += r; r = lcp_sml_buf.load_next_block();
        }
        if (wb%8) {
            lcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
        }
    }

    write_R_output("lcp","construct LCP", "end", 1, 0);
#ifdef STUDY_INFORMATIONS
    std::cout<<"# racs: "<<racs<<std::endl;
    std::cout<<"# matches: "<<matches<<std::endl;
    std::cout<<"# comps2: "<<comps2<<std::endl;
#endif
    return true;
}

bool construct_lcp_goPHI(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
#ifdef STUDY_INFORMATIONS
    size_type racs  = 0; // random accesses to the text
    size_type dcnt1 = 0; // random access character comparisons in the first phase
    size_type dcnt2 = 0; // total character comparisons in the first phase
    size_type comps2 = 0; // comparisons the second phase
#endif
    write_R_output("lcp","construct LCP", "begin", 1, 0);
#ifdef STUDY_INFORMATIONS
    write_R_output("lcp","goPHI phase 1", "begin", 1, 0);
#endif
    construct_bwt(file_map, dir, id);
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    unsigned char* text = NULL;
    if (!util::load_from_int_vector_buffer(text, text_buf)) {  // load text from file system
        throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
    }
    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());   // initialize buffer for suffix array
    const size_type n = sa_buf.int_vector_size;
    const size_type m = 254; // LCP[i] == m+1 corresp. to LCP[i]>= m+1; LCP[i] <= m corresp. to LCP[i] was calculated

    if (n == 0) // if the size of SA is 0 we are already done
        return true;

    size_type cnt_c[257] = {0};   // counter for each character in the text
    size_type cnt_cc[257] = {0};  // prefix sum of the counter cnt_c
    size_type omitted_c[257] = {0};  // counts the omitted occurences for the second phase
    size_type prev_occ_in_bwt[256] = {0};  // position of the previous occurence of each character c in the bwt
    for (size_type i=0; i<256; ++i) prev_occ_in_bwt[i] = (size_type)-1; // initialze the array with -1
    unsigned char alphabet[257] = {0};
    uint8_t sigma = 0;

    size_type nn = 0; // n' for phase 2
    // phase 1: calculate lcp_sml;  memory consumption: 2n bytes (lcp_sml=n bytes, text=n bytes)
    {

        int_vector<8> lcp_sml(n, 0); // initialize array for small values of first phase; note lcp[0]=0
        size_type done_cnt=0;

        for (size_type i=0; i<n; ++i) { // initialize cnt_c
            ++cnt_c[text[i]+1];
        }
        for (int i=1; i<257; ++i) { // calculate sigma and initailize cnt_cc
            if (cnt_c[i] > 0) {
                alphabet[sigma++] = (unsigned char)(i-1);
            }
            cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
        }
        alphabet[sigma] = '\0';
        {
            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // initialize buffer of bwt
            sa_buf.reset();
            sa_buf.load_next_block();
            size_type sai_1 = sa_buf[0];  // store value of sa[i-1]
            size_type r = bwt_buf.load_next_block();
            uint8_t bwti_1 = bwt_buf[0];       // store value of BWT[i-1]
            lcp_sml[ cnt_cc[bwti_1]++ ] = 0;   // lcp_sml[ LF[0] ] = 0
            prev_occ_in_bwt[bwti_1] = 0;  // init previous occurence of character BWT[0]
            ++omitted_c[alphabet[0]];	  //

            int_vector<64> rmq_stack(2*(m+10));   // initialize stack for m+10 elements representing (position, value)
            rmq_stack[0] = 0; rmq_stack[1] = 0;  // first element (-1, -1)
            rmq_stack[2] = 1; rmq_stack[3] = 0;  // second element (0, -1)
            size_type rmq_end=3;				 // index of the value of the topmost element

            uint8_t cur_c = alphabet[1];
//				size_type big_val = 0;
            for (size_type i=1, sai, r_sum=0, cur_c_idx=1, cur_c_cnt=cnt_c[alphabet[1]+1]; r_sum < n;) {
                for (; i < r_sum+r; ++i, --cur_c_cnt) {
                    uint8_t bwti = bwt_buf[i-r_sum];
                    sai = sa_buf[i-r_sum];
                    size_type lf = cnt_cc[bwti];
                    if (!cur_c_cnt) {// cur_c_cnt==0, if there is no more occurence of the current character
                        if (cur_c_cnt < sigma) {
                            cur_c_cnt = cnt_c[(cur_c=alphabet[++cur_c_idx])+1];
                        }
                    }
                    size_type l=0;
                    if (i >= cnt_cc[cur_c]) { // if the current lcp entry is not already done   TODO: schleife von i bis cnt_cc[cur_c]
                        if (bwti == bwti_1 and lf < i) {  // BWT[i]==BWT[i-1]
                            l = lcp_sml[lf] ? lcp_sml[lf]-1 : 0; // l = LCP[LF[i]]-1; l < m+1
                            if (l == m) { // if LCP[LF[i]] == m+1; otherwise LCP[LF[i]] < m+1  the result is correct
                                l += (text[sai_1+m] == text[sai+m]);
#ifdef STUDY_INFORMATIONS
                                if ((sai_1^sai)>>6) // if i and phii are not in the same cache line
                                    ++racs;
#endif
                            }
                            lcp_sml[i] = l;
                            ++done_cnt;
                        } else { // BWT[i] != BWT[i-1] or LF[i] > i
                            if (lf < i)
                                l = lcp_sml[lf] ? lcp_sml[lf]-1 : 0;
#ifdef STUDY_INFORMATIONS
                            if ((sai_1^sai)>>6) // if i and phii are not in the same cache line
                                ++racs;
                            ++dcnt1; ++dcnt2;
#endif
                            while (text[sai_1+l] == text[sai+l] and l < m+1) {
                                ++l;
#ifdef STUDY_INFORMATIONS
                                ++dcnt2;
#endif
                            }
                            lcp_sml[i] = l;
                        }
                    } else { // if already done
                        l = lcp_sml[i];  // load LCP value
                    }
                    /*						if( l > m ){
                    							++big_val;
                    							if( i > 10000 and i < 10500 and big_val > 3000 ){ // if most of the values are big: switch to PHI algorithm
                    								delete [] text; text = NULL;
                    								lcp_sml.resize(0);
                    								bool res = construct_lcp_PHI(file_map, dir, id);
                    								write_R_output("lcp","construct LCP", "end", 1, 0);
                    								return res;
                    							}
                    						}
                    */
                    // invariant: l <= m+1
                    // begin update rmq_stack
                    size_type x = l+1;
                    size_type j = rmq_end;
                    while (x <= rmq_stack[j]) j-=2;  // pop all elements with value >= l
                    rmq_stack[++j] = i+1; // push position i
                    rmq_stack[++j] = x;	  // push value	l
                    rmq_end = j;          // update index of the value of the topmost element
                    if (lf > i) {   // if LF[i] > i, we can calculate LCP[LF[i]] in constant time with rmq
                        ++done_cnt;
                        // rmq query for lcp-values in the interval I=[prev_occ_in_bwt[BWT[i]]+1..i]
                        // rmq is linear in the stack size; can also be implemented with binary search on the stack
                        size_type x_pos = prev_occ_in_bwt[bwti]+2;
                        size_type j = rmq_end-3;
                        while (x_pos <= rmq_stack[j]) j-=2;   //  search smallest value in the interval I
                        lcp_sml[lf] = rmq_stack[j+3] - (rmq_stack[j+3]==m+2); // if lcp-value equals m+1, we subtract 1
                    }
                    if (l > m) {
                        ++nn;
                    } else
                        ++omitted_c[cur_c];

                    prev_occ_in_bwt[bwti] = i;  	 // update previous position information for character BWT[i]
                    ++cnt_cc[bwti];					 // update counter and therefore the LF information
                    sai_1 = sai;					 // update SA[i-1]
                    bwti_1 = bwti;					 // update BWT[i-1]
                }
                r_sum += r; r = bwt_buf.load_next_block();
                sa_buf.load_next_block();
            }
        }
//			delete [] text; text = NULL; // delete text this time in phase 2
        /*
        			if( n > 1000 and nn > 5*(n/6)  ){ // if we would occupy more space than the PHI algorithm => switch to PHI algorithm
        				lcp_sml.resize(0);
        				bool res = construct_lcp_PHI(file_map, dir, id);
        				write_R_output("lcp","construct LCP", "end", 1, 0);
        				return res;
        			}
        */
        if (!util::store_to_file(lcp_sml, (dir+"lcp_sml_"+id).c_str())) {  // store the small LCP values
            throw std::ios_base::failure("cst_construct: Cannot store LCP_sml to file system!");
            return false;
        } else {
            file_map["lcp_sml"] = dir+"lcp_sml_"+id;
        };
    }
#ifdef STUDY_INFORMATIONS
    write_R_output("lcp","goPHI phase 1", "end", 1, 0);
    std::cout<<"# n="<<n<<" nn="<<nn<<" nn/n="<<((double)nn)/n<<std::endl;
    write_R_output("lcp","goPHI phase 2", "begin", 1, 0);
#endif

    // phase 2: calculate lcp_big with PHI algorithm on remaining entries of LCP
    {
        int_vector<> lcp_big(0, 0, bit_magic::l1BP(n-1)+1);//nn, 0, bit_magic::l1BP(n-1)+1);
        {

            write_R_output("lcp","init phi","begin");
#ifdef STUDY_INFORMATIONS
            write_R_output("lcp","initialize todo","begin");
#endif
            size_type sa_n_1 = 0;  // value for SA[n-1]
            bit_vector todo(n,0);  // bit_vector todo indicates which values are > m in lcp_sml
            {
                // initialize bit_vector todo
                int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
                int_vector_file_buffer<> sa_buf(file_map["sa"].c_str()); // load sa
                sa_buf.load_next_block();
                for (size_type i=0, r_sum=0, r = lcp_sml_buf.load_next_block(); r_sum < n;) {
                    if (r > 0)
                        sa_n_1 = sa_buf[r-1];
                    for (; i < r_sum+r; ++i) {
                        if (lcp_sml_buf[i-r_sum] > m) {
                            todo[sa_buf[i-r_sum]] = 1;
                        }
                    }
                    r_sum += r; r = lcp_sml_buf.load_next_block();
                    sa_buf.load_next_block();
                }
            }
//				std::cerr<<"todo="<<todo<<std::endl;
#ifdef STUDY_INFORMATIONS
            write_R_output("lcp","initialize todo","end");
            write_R_output("lcp","initialize todo rank","begin");
#endif
            rank_support_v<> todo_rank(&todo); // initialize rank for todo
#ifdef STUDY_INFORMATIONS
            write_R_output("lcp","initialize todo rank","end");
            write_R_output("lcp","initialize phi'","begin");
#endif

            const size_type bot = sa_n_1;
            int_vector<> phi(nn, bot, bit_magic::l1BP(n-1)+1); // phi

            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // load BWT
            int_vector_file_buffer<> sa_buf(file_map["sa"].c_str()); // load sa
            int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
            sa_buf.load_next_block();
            lcp_sml_buf.load_next_block();
            uint8_t b_1 = 0;
            for (size_type i=0, r_sum=0, r=bwt_buf.load_next_block(),sai_1=0; r_sum < n;) { // initialize phi
                for (; i < r_sum+r; ++i) {
                    uint8_t b = bwt_buf[i - r_sum];  // store BWT[i]
                    size_type sai = sa_buf[i - r_sum];
                    if (lcp_sml_buf[i-r_sum] > m and b != b_1) {  // if i is a big irreducable value
                        phi[todo_rank(sai)] = sai_1;
                    } // otherwise phi is equal to bot
                    b_1 = b;
                    sai_1 = sai;
                }
                r_sum += r;
                r = bwt_buf.load_next_block();
                sa_buf.load_next_block();
                lcp_sml_buf.load_next_block();
            }
#ifdef STUDY_INFORMATIONS
            write_R_output("lcp","initialize phi'","end");
#endif
            write_R_output("lcp","init phi","end");

            write_R_output("lcp","calc plcp","begin");
#ifdef STUDY_INFORMATIONS
            size_type irreducible_nn=0;
#endif
            for (size_type i=0, ii=0, l=m+1,p=0; i < n and ii<nn; ++i) { // execute compact Phi algorithm
                if (todo[i]) {
                    if (i > 0 and todo[i-1])
                        l = l-1;
                    else
                        l = m+1;
                    if ((p=phi[ii]) != bot) {
#ifdef STUDY_INFORMATIONS
                        ++irreducible_nn;
#endif
                        while (text[i+l] == text[p+l]) ++l;
                    }
                    phi[ii++] = l;
                }
            }
            write_R_output("lcp","calc plcp","end");
#ifdef STUDY_INFORMATIONS
            std::cout<<"# irreducible: "<<irreducible_nn<<" nn: "<<nn<<" ratio: "<< ((double)irreducible_nn)/nn <<std::endl;
#endif

            delete [] text; text = NULL;

            write_R_output("lcp","calc lcp","begin");
            lcp_big.resize(nn);
            sa_buf.reset();
            lcp_sml_buf.reset();
            lcp_sml_buf.load_next_block();
            for (size_type i = 0, ii = 0, r = sa_buf.load_next_block(), r_sum = 0; r_sum < n and ii<nn;) {
                for (; i < r_sum+r; ++i) {
                    if (lcp_sml_buf[i - r_sum] > m) {
                        lcp_big[ii++] = phi[todo_rank(sa_buf[i - r_sum])];
                    }
                }
                r_sum += r; r = sa_buf.load_next_block();
                lcp_sml_buf.load_next_block();
            }
            write_R_output("lcp","calc lcp","end");
        }

        if (!util::store_to_file(lcp_big, (dir+"lcp_big_"+id).c_str())) {  // store the big LCP values
            throw std::ios_base::failure("cst_construct: Cannot store LCP_big to file system!");
            return false;
        } else {
            file_map["lcp_big"] = dir+"lcp_big_"+id;
        };
    } // end phase 2
#ifdef STUDY_INFORMATIONS
    write_R_output("lcp","goPHI phase 2", "end", 1, 0);
    write_R_output("lcp","goPHI phase 3", "begin", 1, 0);
#endif

//		std::cout<<"# merge lcp_sml and lcp_big"<<std::endl;
    // phase 3: merge lcp_sml and lcp_big and save to disk
    {
        const size_type buffer_size = 1000000; // buffer_size has to be a multiple of 8!
        int_vector_file_buffer<> lcp_big_buf(file_map["lcp_big"].c_str()); 				// file buffer containing the big LCP values
        int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str(), buffer_size);// file buffer containing the small LCP values

        std::ofstream lcp_out_buf((dir+"lcp_"+id).c_str(), std::ios::binary | std::ios::trunc | std::ios::out);    // open out file stream
        file_map["lcp"] = dir+"lcp_"+id;  																		   // and save result to disk
        int_vector<> lcp_buf(buffer_size, 0, lcp_big_buf.int_width); // buffer for the resulting LCP array
        size_type bit_size = n*lcp_big_buf.int_width;
        lcp_out_buf.write((char*) &(bit_size), sizeof(lcp_big_buf.int_vector_size));		   // write size
        lcp_out_buf.write((char*) &(lcp_big_buf.int_width), sizeof(lcp_big_buf.int_width));   // write int_width
        size_type wb = 0; // written bytes of data

        for (size_type i=0, r_sum=0, r=0, i2=0 ,r2_sum=0, r2=0; r_sum < n;) {
            for (; i < r_sum + r; ++i) {
                size_type l = lcp_sml_buf[i-r_sum];
                if (l > m) { // if l > m it is stored in lcp_big
                    if (i2 >= r2_sum + r2) {
                        r2_sum += r2; r2 = lcp_big_buf.load_next_block();
                    }
                    l = lcp_big_buf[i2-r2_sum];
                    ++i2;
                }
                lcp_buf[i-r_sum] = l;
            }
            if (r > 0) {
                size_type cur_wb = r*lcp_buf.get_int_width();
                if (cur_wb%8 != 0)
                    cur_wb += 7;
                cur_wb /= 8;
                lcp_out_buf.write((const char*)lcp_buf.data(), cur_wb);
                wb += cur_wb;
            }
            r_sum += r; r = lcp_sml_buf.load_next_block();
        }
        if (wb%8) {
            lcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
        }
    }
#ifdef STUDY_INFORMATIONS
    write_R_output("lcp","goPHI phase 3", "end", 1, 0);
#endif
    write_R_output("lcp","construct LCP", "end", 1, 0);
#ifdef STUDY_INFORMATIONS
    std::cout<<"# racs: "<<racs<<std::endl;
    std::cout<<"# dcnt1: "<<dcnt1<<std::endl;
    std::cout<<"# dcnt2: "<<dcnt2<<std::endl;
    std::cout<<"# comps2: "<<comps2<<std::endl;
#endif
    return true;
}

bool construct_lcp_go2(tMSS& file_map, const std::string& dir, const std::string& id)
{
    typedef int_vector<>::size_type size_type;
    write_R_output("lcp","construct LCP", "begin", 1, 0);
    construct_bwt(file_map, dir, id);
    int_vector_file_buffer<8> text_buf(file_map["text"].c_str());
    unsigned char* text = NULL;
    if (!util::load_from_int_vector_buffer(text, text_buf)) {  // load text from file system
        throw std::ios_base::failure("cst_construct: Cannot load text from file system!");
    }
    int_vector_file_buffer<> sa_buf(file_map["sa"].c_str());   // initialize buffer for suffix array
    const size_type n = sa_buf.int_vector_size;
    const size_type m = 254; // LCP[i] == m+1 corresp. to LCP[i]>= m+1; LCP[i] <= m corresp. to LCP[i] was calculated

    if (n == 0) // if the size of SA is 0 we are already done
        return true;

    size_type cnt_c[257] = {0};   // counter for each character in the text
    size_type cnt_cc[257] = {0};  // prefix sum of the counter cnt_c
    size_type cnt_cc2[257] = {0};  //
    size_type omitted_c[257] = {0};  // counts the omitted occurences for the second phase
    size_type prev_occ_in_bwt[256] = {0};  // position of the previous occurence of each character c in the bwt
    for (size_type i=0; i<256; ++i) prev_occ_in_bwt[i] = (size_type)-1; // initialze the array with -1
    unsigned char alphabet[257] = {0};
    uint8_t inv_alphabet[257] = {0};
    uint8_t sigma = 0;

    tLI m_list[2][256];
    size_type m_char_count[2] = {0};
    uint8_t m_chars[2][256] = {{0},{0}};

    size_type nn = 0; // n' for phase 2
    // phase 1: calculate lcp_sml;  memory consumption: 2n bytes ( bytes, text=n bytes)
    {
//			int_vector<8> lcp_sml(n, 0); // initialize array for small values of first phase; note lcp[0]=0

//			int_vector<8> lcp_sml2;
//			util::load_from_file(lcp_sml2, (dir+"lcp_sml_go").c_str() );

        size_type done_cnt=0;

        for (size_type i=0; i<n; ++i) { // initialize cnt_c
            ++cnt_c[text[i]+1];
        }
        for (int i=1; i<257; ++i) { // calculate sigma and initailize cnt_cc
            if (cnt_c[i] > 0) {
                inv_alphabet[i-1] = sigma;
                alphabet[sigma++] = (unsigned char)(i-1);
            }
            cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
        }
        alphabet[sigma] = '\0';
        {


            std::ofstream lcp_sml_out_buf((dir+"lcp_sml_"+id).c_str(), std::ios::binary | std::ios::trunc | std::ios::out);    // open out file stream
            size_type bit_size = 8*n;
            lcp_sml_out_buf.write((char*) &bit_size, sizeof(size_type));    // write size

            buffered_char_queue lcps[256];//lcps[sigma];
            for (size_type i=0; i<sigma; ++i) {
                lcps[i].init(dir, alphabet[i]);
            }

            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // initialize buffer of bwt
            sa_buf.reset();
            sa_buf.load_next_block();
            size_type sai_1 = sa_buf[0];  // store value of sa[i-1]
            size_type r = bwt_buf.load_next_block();
            uint8_t bwti_1 = bwt_buf[0];       // store value of BWT[i-1]
            cnt_cc[bwti_1]++;
            uint8_t temp=0;
            lcp_sml_out_buf.write((char*) &temp, 1);
            lcps[inv_alphabet[bwti_1]].push_back(0);
            lcps[0].push_back(0);
            prev_occ_in_bwt[bwti_1] = 0;  // init previous occurence of character BWT[0]
            ++omitted_c[alphabet[0]];	  //

            int_vector<64> rmq_stack(2*(m+10));   // initialize stack for m+10 elements representing (position, value)
            rmq_stack[0] = 0; rmq_stack[1] = 0;  // first element (-1, -1)
            rmq_stack[2] = 1; rmq_stack[3] = 0;  // second element (0, -1)
            size_type rmq_end=3;				 // index of the value of the topmost element

            const size_type m_mod2 = m%2;
            uint8_t cur_c = alphabet[1];
//				int err_cnt = 0;

            for (size_type i=1, sai, r_sum=0, cur_c_idx=1, cur_c_cnt=cnt_c[alphabet[1]+1]; r_sum < n;) {
                for (; i < r_sum+r; ++i, --cur_c_cnt) {
//						if(n<100) std::cerr<<"i= "<<i<<std::endl;
                    uint8_t bwti = bwt_buf[i-r_sum];
                    sai = sa_buf[i-r_sum];
                    size_type lf = cnt_cc[bwti];
                    if (!cur_c_cnt) {// cur_c_cnt==0, if there is no more occurence of the current character
                        if (cur_c_cnt < sigma) {
                            cur_c_cnt = cnt_c[(cur_c=alphabet[++cur_c_idx])+1];
                        }
                    }
                    size_type l=0;
                    if (i >= cnt_cc[cur_c]) { // if the current lcp entry is not already done   TODO: schleife von i bis cnt_cc[cur_c]
                        if (bwti == bwti_1 and lf < i) {  // BWT[i]==BWT[i-1]
//								l = lcp_sml2[lf] ? lcp_sml2[lf]-1 : 0; // l = LCP[LF[i]]-1; l < m+1

                            l = lcps[inv_alphabet[bwti]].pop_front();
                            l = l ? l-1 : 0;

                            if (l == m) { // if LCP[LF[i]] == m+1; otherwise LCP[LF[i]] < m+1  the result is correct
                                l += (text[sai_1+m] == text[sai+m]);
                            }
                            lcps[cur_c_idx].push_back(l);
                            ++done_cnt;
                        } else { // BWT[i] != BWT[i-1] or LF[i] > i
                            if (lf < i) {
                                l = lcps[inv_alphabet[bwti]].pop_front();
                                l = l ? l-1 : 0;
                            }
                            while (text[sai_1+l] == text[sai+l] and l < m+1) ++l;
                            lcps[cur_c_idx].push_back(l);
                        }
                    } else { // if already done
                        l = lcps[cur_c_idx].pop_front();
                        if (lf < i and bwti != cur_c) { // falls  bwti == cur_c => lf < i?
                            lcps[inv_alphabet[bwti]].pop_front();
                        }
                    }
                    uint8_t wl = l;
                    lcp_sml_out_buf.write((char*)&wl, 1);
                    // invariant: l <= m+1
                    // begin update rmq_stack
                    size_type x = l+1;
                    size_type j = rmq_end;
                    while (x <= rmq_stack[j]) j-=2;  // pop all elements with value >= l
                    rmq_stack[++j] = i+1; // push position i
                    rmq_stack[++j] = x;	  // push value	l
                    rmq_end = j;          // update index of the value of the topmost element
                    if (lf > i) {   // if LF[i] > i, we can calculate LCP[LF[i]] in constant time with rmq
                        ++done_cnt;
                        // rmq query for lcp-values in the interval I=[prev_occ_in_bwt[BWT[i]]+1..i]
                        // rmq is linear in the stack size; can also be implemented with binary search on the stack
                        size_type x_pos = prev_occ_in_bwt[bwti]+2;
                        size_type j = rmq_end-3;
                        while (x_pos <= rmq_stack[j]) j-=2;   //  search smallest value in the interval I
//							lcp_sml2[lf] = rmq_stack[j+3] - (rmq_stack[j+3]==m+2); // if lcp-value equals m+1, we subtract 1
                        lcps[inv_alphabet[bwti]].push_back(rmq_stack[j+3] - (rmq_stack[j+3]==m+2));
//							if(bwti=='A' and i<410){
//								std::cout<<"push A with "<< (int)(rmq_stack[j+3]-(rmq_stack[j+3]==m+2)) << std::endl;
//							}
//							if(n<100)std::cerr<<"push back "<<bwti<<" "<<(int)(rmq_stack[j+3] - (rmq_stack[j+3]==m+2))<<std::endl;
                    }
                    if (l >= m) {
                        if (l == m)
                            push_front_m_index(nn, cur_c, m_list[m_mod2], m_chars[m_mod2], m_char_count[m_mod2]);
                        ++nn;
                    } else
                        ++omitted_c[cur_c];

                    prev_occ_in_bwt[bwti] = i;  	 // update previous position information for character BWT[i]
                    ++cnt_cc[bwti];					 // update counter and therefore the LF information
                    sai_1 = sai;					 // update SA[i-1]
                    bwti_1 = bwti;					 // update BWT[i-1]
                }
                r_sum += r; r = bwt_buf.load_next_block();
                sa_buf.load_next_block();

            }
            if (n % 8) { // add 0 bytes to complete the int_vector
                lcp_sml_out_buf.write((char*)"\0\0\0\0\0\0\0\0", 8 - (n%8));
            }
            lcp_sml_out_buf.close();
            file_map["lcp_sml"] = dir+"lcp_sml_"+id;
//				std::cerr<<"n="<<n<<"\n#(lf>i)="<<done_cnt<<std::endl;
//				std::cerr<<"bwt_buf.int_vector_size="<<bwt_buf.int_vector_size<<std::endl;
//				std::cerr<<"n="<<n<<" nn="<<nn<<" ratio: "<<((double)nn)/n<<std::endl;
        }
        delete [] text; text = NULL;

        if (n > 1000 and nn > 5*(n/6)) {  // if we would occupy more space than the PHI algorithm => switch to PHI algorithm
            bool res = construct_lcp_PHI(file_map, dir, id);
            write_R_output("lcp","construct LCP", "end", 1, 0);
            return res;
        }
    }
    // phase 2: calculate lcp_big
    {
//			std::cout<<"# begin calculating LF' values"<<std::endl;
        int_vector<> lcp_big(nn, 0, bit_magic::l1BP(n-1)+1); // lcp_big first contains adapted LF values and finally the big LCP values
        {
            // initialize lcp_big with adapted LF values
            bit_vector todo(n,0);  // bit_vector todo indicates which values are >= m in lcp_sml
            {
                // initialize bit_vector todo
                int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
                for (size_type i=0, r_sum=0, r = lcp_sml_buf.load_next_block(); r_sum < n;) {
                    for (; i < r_sum+r; ++i) {
                        if (lcp_sml_buf[i-r_sum] >= m) {
                            todo[i] = 1;
                        }
                    }
                    r_sum += r; r = lcp_sml_buf.load_next_block();
                }
            }

            cnt_cc2[0] = cnt_cc[0]= 0;
            for (size_type i=1, omitted_sum=0; i<257; ++i) { // initialize cnt_cc and cnt_cc2
                cnt_cc[i] = cnt_c[i] + cnt_cc[i-1];
                omitted_sum += omitted_c[i-1];
                cnt_cc2[i] = cnt_cc[i] - omitted_sum;
            }

            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // load BWT
            for (size_type i=0, i2=0, r_sum=0, r=bwt_buf.load_next_block(); r_sum < n;) {
                for (; i < r_sum+r; ++i) {
                    uint8_t b = bwt_buf[i - r_sum];  // store BWT[i]
                    size_type lf_i = cnt_cc[b]; // LF[i]
                    if (todo[i]) { // LCP[i] is a big value
                        if (todo[lf_i]) { // LCP[LF[i]] is a big entry
                            lcp_big[i2] = cnt_cc2[b]; // LF'[i]
                        }/*else{
								lcp_big[i2] = 0;
							}*/
                        ++i2;
                    }
                    if (todo[lf_i]) { // LCP[LF[i]] is a big entry
                        ++cnt_cc2[b];	// increment counter for adapted LF
                    }
                    ++cnt_cc[b]; // increment counter for LF
                }
                r_sum += r; r = bwt_buf.load_next_block();
            }
        }

//			std::cout<<"# begin initializing bwt2, shift_bwt2, run2"<<std::endl;
        int_vector<8> bwt2(nn), shift_bwt2(nn); // BWT of big LCP values, and shifted BWT of big LCP values
        bit_vector run2(nn+1);					// indicates for each entry i, if i and i-1 are both big LCP values
        run2[nn] = 0;							// index nn is not a big LCP value
        {
            // initialize bwt2, shift_bwt2, adj2
            int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str()); // load lcp_sml
            int_vector_file_buffer<8> bwt_buf(file_map["bwt"].c_str()); // load BWT
            uint8_t b_1 = '\0'; // BWT[i-1]
            bool is_run = false;
            for (size_type i=0, i2=0, r_sum=0, r = 0; r_sum < n;) {
                for (; i < r_sum+r; ++i) {
                    uint8_t b = bwt_buf[i-r_sum];
                    if (lcp_sml_buf[i-r_sum] >= m) {
                        bwt2[i2] 		= b;
                        shift_bwt2[i2]	= b_1;
                        run2[i2] 		= is_run;
                        is_run 			= true;
                        ++i2;
                    } else {
                        is_run = false;
                    }
                    b_1 = b;
                }
                r_sum += r; r = lcp_sml_buf.load_next_block();
                bwt_buf.load_next_block();
            }
        }

        bit_vector todo2(nn+1, 1); // init all values with 1, except
        todo2[nn] = 0; 			   // the last one! (handels case "i < nn")

//			std::cout<<"# begin calculating m-indices"<<std::endl;
        {
            // calculate m-indices, (m+1)-indices,... until we are done
            size_type m2 = m;
            size_type char_ex[256]; for (size_type i=0; i<256; ++i) char_ex[i] = nn;
            size_type char_occ=0;
            size_type m_mod2 = m2%2, mm1_mod2 = (m2+1)%2;
            while (m_char_count[m_mod2] > 0) { // while there are m-indices, calculate (m+1)-indices and write m-indices
                // For all values LCP[i] >= m2 it follows that todo2[i] == 1
                // list m_list[mm1_mod2][b] is sorted in decreasing order
                ++m2;
                mm1_mod2 = (m2+1)%2, m_mod2 = m2%2;
                m_char_count[m_mod2] = 0;

                std::sort(m_chars[mm1_mod2], m_chars[mm1_mod2]+m_char_count[mm1_mod2]); // TODO: ersetzen?

                for (size_type mc=0; mc<m_char_count[mm1_mod2]; ++mc) { // for every character
                    tLI& mm1_mc_list = m_list[mm1_mod2][m_chars[mm1_mod2][ m_char_count[mm1_mod2]-1-  mc ]];
//						size_type old_i = nn;
                    while (!mm1_mc_list.empty()) {
                        size_type i = mm1_mc_list.front();  // i in [0..n-1]
                        mm1_mc_list.pop_front();
                        // For all values LCP[i] >= m-1 it follows that todo2[i] == 1
                        for (size_type k=i; todo2[k]; --k) {
                            uint8_t b = shift_bwt2[k];
                            if (char_ex[b] != i) {
                                char_ex[b] = i;
                                ++char_occ;
                            }
                            if (!run2[k])
                                break;
                        }
                        for (size_type k=i; todo2[k] and char_occ; ++k) {
                            uint8_t b = bwt2[k];
                            if (char_ex[b] == i) {
                                size_type p = lcp_big[k];
                                push_back_m_index(p, b, m_list[m_mod2], m_chars[m_mod2], m_char_count[m_mod2]);
                                char_ex[b] = nn;
                                --char_occ;
                            }
                            if (!run2[k+1])
                                break;
                        }
                        lcp_big[ i ] = m2-1;
                        todo2[ i ] = 0;
//							old_i = i;
                    }
                }
            }

        }

        if (!util::store_to_file(lcp_big, (dir+"lcp_big_"+id).c_str())) {  // store the big LCP values
            throw std::ios_base::failure("cst_construct: Cannot store LCP_big to file system!");
            return false;
        } else {
            file_map["lcp_big"] = dir+"lcp_big_"+id;
        };
    } // end phase 2

//		std::cout<<"# merge lcp_sml and lcp_big"<<std::endl;
    // phase 3: merge lcp_sml and lcp_big and save to disk
    {
        const size_type buffer_size = 1000000; // buffer_size has to be a multiple of 8!
        int_vector_file_buffer<> lcp_big_buf(file_map["lcp_big"].c_str()); 				// file buffer containing the big LCP values
        int_vector_file_buffer<8> lcp_sml_buf(file_map["lcp_sml"].c_str(), buffer_size);// file buffer containing the small LCP values

        std::ofstream lcp_out_buf((dir+"lcp_"+id).c_str(), std::ios::binary | std::ios::trunc | std::ios::out);    // open out file stream
        file_map["lcp"] = dir+"lcp_"+id;  																		   // and save result to disk
        int_vector<> lcp_buf(buffer_size, 0, lcp_big_buf.int_width); // buffer for the resulting LCP array
        size_type bit_size = n*lcp_big_buf.int_width;
        lcp_out_buf.write((char*) &(bit_size), sizeof(lcp_big_buf.int_vector_size));		   // write size
        lcp_out_buf.write((char*) &(lcp_big_buf.int_width), sizeof(lcp_big_buf.int_width));   // write int_width
        size_type wb = 0; // written bytes of data

        for (size_type i=0, r_sum=0, r=0, i2=0 ,r2_sum=0, r2=0; r_sum < n;) {
            for (; i < r_sum + r; ++i) {
                size_type l = lcp_sml_buf[i-r_sum];
                if (l >= m) { // if l >= m it is stored in lcp_big
                    if (i2 >= r2_sum + r2) {
                        r2_sum += r2; r2 = lcp_big_buf.load_next_block();
                    }
                    l = lcp_big_buf[i2-r2_sum];
                    ++i2;
                }
                lcp_buf[i-r_sum] = l;
            }
            if (r > 0) {
                size_type cur_wb = r*lcp_buf.get_int_width();
                if (cur_wb%8 != 0)
                    cur_wb += 7;
                cur_wb /= 8;
                lcp_out_buf.write((const char*)lcp_buf.data(), cur_wb);
                wb += cur_wb;
            }
            r_sum += r; r = lcp_sml_buf.load_next_block();
        }
        if (wb%8) {
            lcp_out_buf.write("\0\0\0\0\0\0\0\0", 8-wb%8);
        }
    }

    write_R_output("lcp","construct LCP", "end", 1, 0);

    return true;
}


void check_lcp(std::string lcpI, std::string lcpII, std::string id)
{
    typedef int_vector<>::size_type size_type;
    int_vector<> lcp1,lcp2;
    util::load_from_file(lcp1, (lcpI+"_"+id).c_str());
    util::load_from_file(lcp2, (lcpII+"_"+id).c_str());
    if (lcp1 != lcp2) {
        std::cout<<"lcp results of "<<  lcpI << "and " <<lcpII<<" differ"<<std::endl;
        for (size_type i=0, cnt=0; i<lcp1.size() and cnt<10; ++i) {
            if (lcp1[i] != lcp2[i]) {
                std::cout<<"i="<<i<<" "<<lcpI<<"[i]="<<lcp1[i]<<" "<<lcpII<<"[i]="<<lcp2[i]<<std::endl;
                ++cnt;
            }
        }
    }
}

void lcp_info(tMSS& file_map)
{
    typedef int_vector<>::size_type size_type;
    int_vector_file_buffer<> lcp_buf(file_map["lcp"].c_str());
    size_type n = lcp_buf.int_vector_size;

    size_type max_lcp = 0;
    size_type sum_lcp = 0;
    for (size_type i=0, r_sum=0, r=0; i < n;) {
        for (; i < r_sum+r; ++i) {
            if (lcp_buf[i-r_sum] > max_lcp)
                max_lcp = lcp_buf[i-r_sum];
            sum_lcp += lcp_buf[i-r_sum];
        }
        r_sum += r; r = lcp_buf.load_next_block();
    }
    std::cout<<"# max lcp = " << max_lcp << std::endl;
    std::cout<<"# sum lcp = " << sum_lcp << std::endl;
    std::cout<<"# avg lcp = " << sum_lcp/(double)n << std::endl;
}



} // end namespace sdsl
