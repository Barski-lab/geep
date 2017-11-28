//
// Created by kot4or on 8/8/16.
//

#include "annotation_reader.h"

map <string, string> split_attributes (string line){
    map <string, string> attribute_map;
    vector<string> attributes_array = split_line(line, "; ");
    assert (attributes_array.size() % 2 == 0); // check if attributes forms key value structure
    for (int i = 0; i < attributes_array.size(); i+=2){
        if ( attributes_array[i+1].length() >= 2 ){ // If we can delete doublequotes
            attribute_map[attributes_array[i]] = string (attributes_array[i+1], 1, attributes_array[i+1].length()-2);
        } else {
            attribute_map[attributes_array[i]] = attributes_array[i+1];
        }
    }
    return attribute_map;
};

void rearrange_array_from_gtf (vector<string> & input){
    vector<string> output;

    output.push_back("0"); //         0: bin                   (long)
    output.push_back(input[9]); //  1: name                  (string)
    output.push_back(input[0]); //  2: chrom                 (string)
    output.push_back(input[6]); //  3: strand                (+/-)
    output.push_back(input[3]); //  4: tx_start              (long)  !!! Equal to exon_starts. In a case of GTF it will be only one value
    output.push_back(input[4]); //  5: tx_end                (long)  !!! Equal to exon_ends. In a case of GTF it will be only one value
    output.push_back("0"); //         6: cds_start             (long)
    output.push_back("0"); //         7: cds_end               (long)
    output.push_back("1"); //         8: exon_count            (long)
    output.push_back(input[3]); //  9: exon_starts           (long,long,long)
    output.push_back(input[4]); // 10: exon_ends             (long,long,long)
    output.push_back(input[5]); // 11: score                 (double)
    output.push_back(input[8]); // 12: name2                 (string)
    output.push_back("none"); //        13: cds_start_stat   (enum: none - default)
    output.push_back("none"); //        14: cds_end_stat     (enum: none - default)
    output.push_back("0"); //        15: exon_frames      (long,long,long)

    input = output;
}


void Isoform::print (){
    cout << "bin: " << bin << endl;
    cout << "isoform: " << name << endl;
    cout << "chrom: " << chrom << endl;
    cout << "name: " << name << endl;
    cout << "strand: " << strand << endl;
    cout << "[tx_start, tx_end]: [" << tx_start << ", " << tx_end << "]" << endl;
    cout << "[cds_start, cds_end]: [" << cds_start << ", " << cds_end << "]" << endl;
    cout << "exon_count: " << exon_count << endl;
    cout << "length: " << length << endl;
    cout << "total_reads: " << total_reads << endl;
    cout << "density: " << density << endl;
    cout << "rpkm: " << rpkm << endl;
    cout << "index: " << index << endl;
    cout << "exon_starts.size(): " << exon_starts.size() << endl;
    cout << "exon_ends.size(): " << exon_ends.size() << endl;
//    cout << "exon_frames.size(): " << exon_frames.size() << endl;
    cout << "Exons starts:" << endl;
    for (auto it = exon_starts.begin(); it != exon_starts.end(); ++it){
        cerr << *it << endl;
    }
    cout << "Exons ends:" << endl;
    for (auto it = exon_ends.begin(); it != exon_ends.end(); ++it){
        cerr << *it << endl;
    }
//    for (int i = 0; i < exon_count; i++){
//        cout << "  " << i << ") " << "[" << exon_starts[i] << ", "<< exon_ends[i] << "] - " << endl;
//    }
    cout << "score: " << score << endl;
    cout << "name2: " << name2 << endl;
    cout << "cds_start_stat: " << cds_start_stat << endl;
    cout << "cds_end_stat: " << cds_end_stat << endl;
}


Isoform::Isoform (string line, bool gtf):
         bin (0)
        ,name ("")
        ,chrom("")
        ,strand(true)
        ,tx_start(0)
        ,tx_end(0)
        ,cds_start(0)
        ,cds_end(0)
        ,exon_count(0)
        ,length (0)
        ,total_reads(0)
        ,density(0)
        ,rpkm(0)
        ,index(0)
        ,score(0)
        ,name2("")
        ,cds_start_stat (cds_stat::none)
        ,cds_end_stat (cds_stat::none)
        ,cycles(0)
        ,bin_id (".")
{

    vector<string> line_splitted = split_line(line);

    if (gtf){
        if (line_splitted[2] != "exon"){
            throw ("Isoform constructor error: not exon");
        }
        map <string, string> attributes_map = split_attributes(line_splitted[8]);
        line_splitted.pop_back();
        line_splitted.push_back(attributes_map["gene_id"]);
        line_splitted.push_back(attributes_map["transcript_id"]);
        rearrange_array_from_gtf (line_splitted);
    }

//    for (int i = 0; i < line_splitted.size(); i++){
//        cerr << i << ". " << line_splitted[i] << endl;
//    }
//    cerr << endl;

    // BIN
    if (not str_to_int(bin, line_splitted[0])){
        throw ("Isoform constructor error");
    }

    // NAME
    name = line_splitted[1];

    // CHROM
    chrom = line_splitted[2];

    // STRAND
    strand = (line_splitted[3] == "+") ? true : false;

    // TX_START
    if (not str_to_long(tx_start, line_splitted[4])){
        throw ("Isoform constructor error");
    }

    // TX_END
    if (not str_to_long(tx_end, line_splitted[5])){
        throw ("Isoform constructor error");
    }

    // CDS_START
    if (not str_to_long(cds_start, line_splitted[6])){
        throw ("Isoform constructor error");
    }

    //CDS_END
    if (not str_to_long(cds_end, line_splitted[7])){
        throw ("Isoform constructor error");
    }

    // EXON_COUNT
    if (not str_to_int(exon_count, line_splitted[8])){
        throw ("Isoform constructor error");
    }

    //SCORE
    if (not str_to_double(score, line_splitted[11])){
        if (line_splitted[11] == "."){
            cout << "Score set to 0" << endl;
            score = 0;
        } else {
            throw ("Isoform constructor error");
        }
    }

    // NAME2
    name2 = line_splitted[12];

    // CDS_START_STAT
    if (not str_to_cds_stat(line_splitted[13], cds_start_stat)){
        throw ("Cannot set the value for cds_start_stat");
    }

    // CDS_END_STAT
    if (not str_to_cds_stat(line_splitted[14], cds_end_stat)){
        throw ("Cannot set the value for cds_end_stat");
    }

    vector<string> exon_starts_str = split_line(line_splitted[9], ",");
    vector<string> exon_ends_str = split_line(line_splitted[10], ",");
    vector<string> exon_frames_str = split_line(line_splitted[15], ",");

    //        print_vector (exon_starts_str, "exon_starts_str");
    //        print_vector(exon_ends_str, "exon_ends_str");
    //        print_vector(exon_frames_str, "exon_frames_str");


    if (not str_array_to_set(exon_starts_str, exon_starts)){
        throw ("Isoform class constructor fail");
    };

    if (not str_array_to_set(exon_ends_str, exon_ends)){
        throw ("Isoform class constructor fail");
    };

//    if (not str_array_to_long_array(exon_frames_str, exon_frames)){
//        throw ("Isoform class constructor fail");
//    };

    if (gtf){ // update coordinates if GTF TODO double check if it's correct to do like this
        tx_start = tx_start - 1; // Changing the TSS to correspond 0-based corodinate system
        set <long> exon_starts_new;
        for (auto it = exon_starts.begin(); it != exon_starts.end(); ++it){
            exon_starts_new.insert (*it-1);
        }
        exon_starts = exon_starts_new;
    }

    set <long>::iterator start_it = exon_starts.begin();
    set <long>::iterator stop_it = exon_ends.begin();
    // calculating the length of isoform as sum of all exons' lengths
    for (int i = 0; i < exon_count; i++){
        assert (exon_starts.size() == exon_count && exon_ends.size() == exon_count);
        length += (long)((*stop_it) - (*start_it));
        start_it++;
        stop_it++;
    }

}

Isoform::Isoform ():
        bin (0)
        ,name ("")
        ,chrom("")
        ,strand(true)
        ,tx_start(0)
        ,tx_end(0)
        ,cds_start(0)
        ,cds_end(0)
        ,exon_count(0)
        ,length (0)
        ,total_reads(0)
        ,density(0)
        ,rpkm(0)
        ,index(0)
        ,score(0)
        ,name2("")
        ,cds_start_stat (cds_stat::none)
        ,cds_end_stat (cds_stat::none)
        ,cycles (0)
        ,bin_id (".")
{

};

Isoform& Isoform::operator+=(const Isoform& other_iso){
    if (name  != other_iso.name  ||
        name2 != other_iso.name2 ||
        chrom != other_iso.chrom ||
        strand != other_iso.strand){

        cerr << "Isoform += operator error. Parameters mismatch" <<endl;
        cerr << name << " - " <<  other_iso.name << endl;
        cerr << name2 << " - " <<  other_iso.name2 << endl;
        cerr << chrom << " - " <<  other_iso.chrom << endl;
        cerr << strand << " - " <<  other_iso.strand << endl;

        return *this;
    }
    exon_count += other_iso.exon_count;
    length += other_iso.length;
    exon_starts.insert(other_iso.exon_starts.begin(), other_iso.exon_starts.end());
    exon_ends.insert(other_iso.exon_ends.begin(), other_iso.exon_ends.end());
//    exon_frames.insert(exon_frames.end(), other_iso.exon_frames.begin(), other_iso.exon_frames.end());
    tx_start = *exon_starts.begin();
    tx_end = *exon_ends.rbegin();
    return *this;
}


bool str_to_cds_stat(const string &value, cds_stat &result){
    if (value == "none") {
        result = none;
    } else
    if (value == "unk"){
        result = unk;
    } else
    if (value == "incmpl"){
        result = incmpl;
    } else
    if (value == "cmpl"){
        result = cmpl;
    } else {
        cout << "Cannot evalueate " << value << endl;
        return false;
    }
    return true;
}

void print_iso_var_map (const std::map <string, std::map <string, Isoform> > & iso_var_map){
    for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
        cout << "Chromosome: " << ext_it->first << endl;
        for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
            cout << setw(10) << "  isoform: "
                 << setw(15) << int_it->first
                 << setw(10) << " index: "
                 << setw(3) << int_it->second.index
                 << setw(10) << " length: "
                 << setw(5) << int_it->second.length
                 << setw(15) << " total_reads: "
                 << setw(5) << int_it->second.total_reads
                 << setw(10) << " density: "
                 << setw(15) << int_it->second.density
                 << setw(10) << " rpkm: "
                 << setw(5) << int_it->second.rpkm << endl;
        }
    }
}

void print_iso_var_map_to_file (const std::map <string, std::map <string, Isoform> > & iso_var_map, const string path){
    ofstream output_stream (path);
    if (output_stream.is_open())
    {
        output_stream // header line
                << "#"
                << "isoform" << "\t"
                << "chrom" << "\t"
                << "gene" << "\t"
                << "index" << "\t"
                << "length" << "\t"
                << "total_reads" << "\t"
                << "density" << "\t"
                << "rpkm"<< "\t"
                << "cycles" << "\t"
                << "bin_id" << endl;
        for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
            for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
                output_stream
                     << int_it->first << "\t"
                     << ext_it->first << "\t"
                     << int_it->second.name2 << "\t"
                     << int_it->second.index << "\t"
                     << int_it->second.length << "\t"
                     << int_it->second.total_reads << "\t"
                     << int_it->second.density << "\t"
                     << int_it->second.rpkm << "\t"
                     << int_it->second.cycles << "\t"
                     << int_it->second.bin_id << endl;
            }
        }
        output_stream.close();
        cerr << "Results are successfully exported to " << path << endl;
    }
    else cout << "Unable to open output file: " << path << endl;
}


void export_isoform_group (const std::map <string, std::map <string, Isoform> > & iso_var_map, const string path){
    ofstream output_stream (path);
    if (output_stream.is_open())
    {
        output_stream // header line
                << "RefseqId" << ","
                << "GeneId" << ","
                << "Chrom" << ","
                << "TxStart" << ","
                << "TxEnd" << ","
                << "Strand" << ","
                << "TotalReads" << ","
                << "Rpkm" << endl;
        for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
            for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
                std::string strand_str = int_it->second.strand?"+":"-";
                output_stream
                        << int_it->second.name << "," // RefseqId
                        << int_it->second.name2 << "," // GeneId
                        << ext_it->first << "," // Chrom
                        << int_it->second.tx_start << ","
                        << int_it->second.tx_end << ","
                        << strand_str << ","
                        << int_it->second.total_reads << ","
                        << int_it->second.rpkm << endl;
            }
        }
        output_stream.close();
        cerr << "Results are successfully exported to " << path << endl;
    }
    else cout << "Unable to open output file: " << path << endl;
}


std::map <string, std::map <string, Isoform> > group_by_gene (const std::map <string, std::map <string, Isoform> > & iso_var_map){
    // map to save <chromosome name, <gene name, correspondent Isoform object> >
    std::map <string, std::map <string, Isoform> > gene_var_map;
    for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
        for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
            pair <string, Isoform> internal_pair_for_gene_var_map (int_it->second.name2, int_it->second);
            std::map <string, Isoform> internal_gene_var_map;
            internal_gene_var_map.insert(internal_pair_for_gene_var_map);
            pair <std::map <string, std::map <string, Isoform> >::iterator, bool> res;
            pair <string, std::map <string, Isoform> > external_pair_for_gene_var_map (ext_it->first, internal_gene_var_map);
            res = gene_var_map.insert (external_pair_for_gene_var_map);
            if ( !res.second ){
                pair <std::map <string, Isoform>::iterator, bool> insert_gene_res;
                insert_gene_res = res.first->second.insert(internal_pair_for_gene_var_map);
                if ( !insert_gene_res.second ){
                    // we already have this gene in our map std::map <string, Isoform>
                    insert_gene_res.first->second.name = insert_gene_res.first->second.name + " " + int_it->second.name; // Concatenate names of two isofroms
                    insert_gene_res.first->second.total_reads = insert_gene_res.first->second.total_reads + int_it->second.total_reads; // Sum total reads number
                    insert_gene_res.first->second.rpkm = insert_gene_res.first->second.rpkm + int_it->second.rpkm; // Sum rpkm values
//                    assert (insert_gene_res.first->second.tx_start == int_it->second.tx_start); // suppose to be the same for the gene
//                    assert (insert_gene_res.first->second.tx_end == int_it->second.tx_end); // suppose to be the same for the gene
                }
            }
        }
    }
    return gene_var_map;
};

void export_gene_group (const std::map <string, std::map <string, Isoform> > & iso_var_map, const string path){
    std::map <string, std::map <string, Isoform> > gene_var_map = group_by_gene (iso_var_map);
    export_isoform_group (gene_var_map, path);
}




bool is_duplicate (const Isoform & original_isoform, const Isoform & new_isoform, bool gtf){
    if (not gtf){
        // if not gtf, than we work with tab delimited file, where each line is one isoform
        // In this case it's not allowed to have few lines with the same isoform name inside one chromosome
        // We call this function only after we noticed that isoform name already exist in our map
        // So we don't need to check any additional rules to return true - to update the name of the isoform
        // This works good for two situation: two isoforms have the same name and strand, two isoforms have the same name,
        // but different strand
        return true;
    } else {
        // if we work with gtf file we need to check if strand differs from the strand of existing isoform with the same name in our map
        // If strand is not the same, we need to return true to update the name of the isofirm
        // If strand is the same it means, that we process the next exon from the same isoform and we don't need to change the isoform name
        if (original_isoform.strand != new_isoform.strand){
            return true;
        } else {
            return false;
        }
    }
}


// global_annotation_map_ptr : key - chromosome name, value - multimap of annotations, sorted by not-unique key - start pose of annotation
// NOTE : forward list of annotations should be sorted by start pose with rule a<b
bool load_annotation (const string & full_path_name,
                      const string & exclude_chr_coma_sep_list,
                      std::map <string, multimap <long, GffRecordPtr> > & global_annotation_map_ptr,
                      std::map <string, std::map <string, Isoform> > & iso_var_map){
    ifstream input_stream (full_path_name);
    if (!input_stream) {
        cout << "Cannot open file " << full_path_name << endl;
        return false;
    }

    vector<string> exclude_chr;
    try {
        exclude_chr = split_line( boost::to_lower_copy(exclude_chr_coma_sep_list), "," );
    } catch (...){
        cerr << "Cannot interpret exclude chromosome list [" << exclude_chr_coma_sep_list << "]. Set to default empty value" << endl;
    }

    // check if annotation input is in gtf format
    bool gtf = false;
    string annotation_file_ext (full_path_name, full_path_name.find_last_of('.')+1);
    if (annotation_file_ext == "gtf"){
        cerr << "Set to read from gtf annotation file format" << endl;
        gtf = true;
    } else {
        cerr << "Set to read from tab delimited annotation file format" << endl;
    }

    string line;
    while (getline(input_stream, line)) {
        if (string_tools::include_key(line, "#")) { // to filter commented lines
            cout << "Filtered line: " << line << endl;
            continue;
        }
        Isoform current_isoform;
        try {
            current_isoform = Isoform (line, gtf);
        } catch (...){
            cout << "Skipped line [" << line << "]" << endl;
            continue;
        }

        if (std::find(exclude_chr.begin(), exclude_chr.end(), current_isoform.chrom) != exclude_chr.end()){
            cout << "Skip chromosome [" << current_isoform.chrom << "]" << endl;
            continue;
        }

        pair <string, Isoform> internal_pair_for_iso_var_map (current_isoform.name, current_isoform);
        std::map <string, Isoform> internal_iso_var_map;
        internal_iso_var_map.insert(internal_pair_for_iso_var_map);

        pair <std::map <string, std::map <string, Isoform> >::iterator, bool> res;
        pair <string, std::map <string, Isoform> > external_pair_for_iso_var_map (current_isoform.chrom, internal_iso_var_map);
        res = iso_var_map.insert (external_pair_for_iso_var_map);
        if ( !res.second ){
            bool isoform_in_process = true;
            int i = 0; // variable to add to the end of isoform name, when duplicate is allowed
            while (isoform_in_process){
                pair <std::map <string, Isoform>::iterator, bool> insert_iso_res;
                insert_iso_res = res.first->second.insert(internal_pair_for_iso_var_map);
                if ( !insert_iso_res.second ){
                    i++;
                    if (is_duplicate (insert_iso_res.first->second, current_isoform, gtf)){
                        // need to update isoform name in current_isoform
                        size_t dup_pos = current_isoform.name.find ("_dup");
                        if (dup_pos == string::npos){
                            // This is the first time when we rename the isoform
                            // We should add _dup suffix
                            current_isoform.name = current_isoform.name + "_dup" + to_string(i);
                        } else {
                            // Out isoform name already have _dup suffix, so we need to update only the number after _dup
                            current_isoform.name = current_isoform.name.substr(0, dup_pos) + "_dup" + to_string(i);
                        }
                        cerr << "Duplicate isoform name. Name is updated to " << current_isoform.name << endl;
                        internal_pair_for_iso_var_map.first = current_isoform.name;
                        internal_pair_for_iso_var_map.second = current_isoform;
                        continue;
                    }
                    // we already have this isoform in our map std::map <string, Isoform>
                    insert_iso_res.first->second += current_isoform;
                }
                isoform_in_process = false;
            }
        }
    }

//
//    for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
//        cerr << "Chromosome: " << ext_it->first << endl;
//        for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
//            cout << setw(10) << "  isoform: " << setw(15) << int_it->first << endl;
//            int_it->second.print();
//        }
//    }




//    GffRecordPtr previous_annotation;
//    previous_annotation.reset();

    for (auto ext_it = iso_var_map.begin(); ext_it != iso_var_map.end(); ++ext_it){
        for (auto int_it = ext_it->second.begin(); int_it != ext_it->second.end(); ++int_it){
            GffRecordPtr previous_annotation;
            previous_annotation.reset();
            set <long>::iterator start_it = int_it->second.exon_starts.begin();
            set <long>::iterator stop_it = int_it->second.exon_ends.begin();
            for (int i = 0; i < int_it->second.exon_count; i++){
                stringstream ss;
                ss << i+1;
                string exon_id = ss.str();
                bool start_ex = false;
                bool stop_ex = false;
                if (i == 0) start_ex = true;
                if (i == int_it->second.exon_count-1) stop_ex = true;

                GffRecordPtr current_gff (new GffRecord (*start_it, *stop_it, exon_id, int_it->second.name, previous_annotation, int_it->second.strand, start_ex, stop_ex) );
                previous_annotation = current_gff;
                pair <long, GffRecordPtr> internal_pair (*start_it, current_gff);
                multimap <long, GffRecordPtr> internal_multimap;
                internal_multimap.insert (internal_pair);
                pair <std::map <string, multimap <long, GffRecordPtr> >::iterator, bool> ret;
                pair <string, multimap <long, GffRecordPtr> > external_pair (int_it->second.chrom, internal_multimap);
                ret = global_annotation_map_ptr.insert (external_pair);
                if (ret.second == false) {
                    ret.first->second.insert (internal_pair);
                }
                start_it++;
                stop_it++;
            }
            // clear exon_starts, exon_ends, exon_frames from iso_map, because we have all that data in global_annotation_map_ptr
            int_it->second.exon_starts.clear();
            int_it->second.exon_ends.clear();
//            int_it->second.exon_frames.clear();
        }
    }


    return true;
}