#include "setup_tests.hh"
#include "kmc_construct.hh"
#include "globals.hh"
#include "sequence_writers.hh"
#include <gtest/gtest.h>

void create_rc_file_test(const string& file_extension){

    vector<string> seqs1 = {"ACAGT", "CGAG", "CGGACG"};
    vector<string> seqs2 = {"AGAT", "GAGA", "AAAAAA"};
    string f1 = get_temp_file_manager().create_filename("",file_extension);
    string f2 = get_temp_file_manager().create_filename("",file_extension);
    vector<string> oldfiles = {f1,f2};

    // Write to files
    Sequence_Writer_Buffered w1(f1);
    Sequence_Writer_Buffered w2(f2);
    for(string seq : seqs1) w1.write_sequence(seq.c_str(), seq.size());
    for(string seq : seqs2) w2.write_sequence(seq.c_str(), seq.size());
    w1.flush();
    w2.flush();

    // Create file list
    string filelist = get_temp_file_manager().create_filename("",".txt");
    throwing_ofstream filelist_out(filelist);
    filelist_out.stream << f1 << "\n" << f1 << "\n";
    filelist_out.close();

    // Reverse complement
    vector<string> newfiles = create_reverse_complement_files(oldfiles);
    ASSERT_EQ(newfiles.size(), oldfiles.size());

    // Check
    for(LL i = 0; i < newfiles.size(); i++){
        Sequence_Reader sr1(oldfiles[i]);
        Sequence_Reader sr2(newfiles[i]);
        while(!sr1.done()){
            string s1 = sr1.get_next_query_stream().get_all();
            ASSERT_FALSE(sr2.done());
            string s2 = sr2.get_next_query_stream().get_all();
            logger << s1 << endl << get_rc(s2) << endl << "--" << endl;
            ASSERT_EQ(s1, get_rc(s2));
        }
        ASSERT_TRUE(sr2.done());
    }
 
}

TEST(MISC, create_rc_files){
    create_rc_file_test(".fna");
    create_rc_file_test(".fq");
}