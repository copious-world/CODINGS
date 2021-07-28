

#include <cmath>
#include <list>
#include <deque>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;


#include "conversions.hpp"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main(int arc, char **argv) {
    string aa = "add symb: ";

    const unsigned char *instr = (const unsigned char *)"FORE SCORE AND SEVEN YEARS AGO, OUR FOUNDING FATHERS CAME UPON THIS NATION";
    unsigned char buffer[2046];
    unsigned char final_buffer[2046];

    size_t in_len = strlen((const char *)instr);
    size_t out_len = 0;

    cout << "LEN: " << in_len << " :: ";
    cout << "Input string: " << instr << endl;

    // binary
    cout << "BINARY" << endl;
    convert_to_binary_str(instr,in_len,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    //
    convert_from_binary_str((const unsigned char *)buffer,out_len,final_buffer);
    cout << final_buffer << endl;
    //
    //
    // octal
    cout << "OCTAL" << endl;
    convert_to_octal_str(instr,in_len,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    //
    convert_from_octal_str((const unsigned char *)buffer,out_len,final_buffer);
    cout << final_buffer << endl;
    //
    // hex
    cout << "HEX" << endl;
    convert_to_hex_str(instr,in_len,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    //
    convert_from_hex_str((const unsigned char *)buffer,out_len,final_buffer);
    cout << final_buffer << endl;
    //
    // 58
    cout << "B58" << endl;
    convert_to_58_str(instr,in_len,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    //
    convert_from_58_str((const unsigned char *)buffer,out_len,final_buffer,2046);
    cout << final_buffer << endl;
    //
    cout << "B58" << endl;
    const unsigned char *instr2 = (const unsigned char *)"Hello World";
    size_t in_len2 = strlen((const char *)instr2);
    //
    convert_to_58_str(instr2,in_len2,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    convert_from_58_str((const unsigned char *)buffer,out_len,final_buffer,2046);
    cout << final_buffer << endl;
    //
    // b64
    cout << "B64" << endl;
    convert_to_64_str(instr,in_len,buffer);
    out_len = strlen((const char *)buffer);
    cout << "LEN: " << out_len << " :: ";
    cout << buffer << endl;
    //
    convert_from_64_str((const unsigned char *)buffer,out_len,final_buffer);
    cout << final_buffer << endl;

    //
    cout << "DONE" << endl;

    return 0;
}
