//
// Created by kot4or on 8/8/16.
//

#include "bam_reader.h"
#include "string_tools.h"


#ifndef TEST_1_ANNOTATION_READER_H
#define TEST_1_ANNOTATION_READER_H
#endif //TEST_1_ANNOTATION_READER_H

using namespace std;
using namespace string_tools;


enum cds_stat {
    none,
    unk,
    incmpl,
    cmpl
};


class GffRecord;
typedef boost::shared_ptr<GffRecord> GffRecordPtr;

class GffRecord {
public:
    long start_pose; // start position of annotation
    long end_pose; // stop position of annotation
    string exon_id; // text-identificator of current annotation. Looks like we will use it only for debug
    string isoform_id; // set the name of the isofom=rm to which current annotation belongs
    vector < BamRecordPtr > bam_records; // array of pointers to all of the reads, which belongs to this annotation. Need this for debug, than we can delete this field
    long reads_count; // total number of reads, which belongs to this annotation
    GffRecordPtr previous_gff; // ptr to the previous annotation in the same isoform. In NULL - first annotation in current isiform

    // CONSTRUCTOR WITH PARAMETERS
    GffRecord (long start, long end, string exon, string isoform, GffRecordPtr pre_gff)
            : start_pose (start)
            , end_pose (end)
            , exon_id (exon)
            , isoform_id (isoform)
            , previous_gff (pre_gff)
            , reads_count (0)
    {}

    // EMPTY CONSTRUCTOR
    GffRecord ()
            : start_pose (0)
            , end_pose (0)
            , exon_id ("")
            , isoform_id ("")
            , previous_gff (NULL)
            , reads_count (0)
    {}

};

class Isoform {
public:
    int bin; // 0
    string name; // 1
    string chrom; // 2
    bool strand;  // true - +, false  - - // 3
    long tx_start; // 4
    long tx_end; // 5
    long cds_start; // 6
    long cds_end; // 7
    int exon_count; // 8

    vector <long> exon_starts; // not necessary to be sorted // 9
    vector <long> exon_ends; // saves pointers to ends of the exons. not necessary to be sorted // 10
    vector <long> exon_frames; // pointers to exon frames, not necessary to be sorted // 15

    int score; //11
    string name2; // 12
    cds_stat cds_start_stat; // 13
    cds_stat cds_end_stat; // 14

    void print ();

    // Constructor
    Isoform (string line);
};

bool str_to_cds_stat(const string &value, cds_stat &result);

void print_iso_var_map (const std::map <string, std::map <string, int> > & iso_var_map);

// global_annotation_map_ptr : key - chromosome name, value - multimap of annotations, sorted by not-unique key - start pose of annotation
// NOTE : forward list of annotations should be sorted by start pose with rule a<b
bool load_annotation (const string & full_path_name,
                      std::map <string, multimap <long, GffRecordPtr> > & global_annotation_map_ptr,
                      std::map <string, std::map <string, int> > & iso_var_map);