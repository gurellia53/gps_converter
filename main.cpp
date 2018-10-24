#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <math.h>

using namespace std;

namespace
{
const double R_EARTH = 6.371e6; //meters
const unsigned DISTANCE_LINE_OFFSET = 2;
const unsigned COORDINATE_LINE_OFFSET = 3;

struct Coordinates
{
    public:
    Coordinates(double Lat, double Lon): lat(Lat), lon(Lon)
    {}
    Coordinates(): lat(0), lon(0)
    {}
    double lat;
    double lon;
};

class Movement
{
public:
    Movement(double North, double East): north(North), east(East)
    {}
    Movement(): north(0), east(0)
    {}
    double north;
    double east;
};

bool IsDataRow( std::string line )
{
    if( line.substr(0,2) == "  " )
    {
        return true;
    }
    return false;
}

bool IsHeaderRow( bool IsDataRow, bool WasDataRow)
{
    if( !IsDataRow && WasDataRow )
    {
        return true;
    }
    return false;
}

bool IsDistanceRow( unsigned line_num, unsigned last_header_line )
{
    if ( line_num == (last_header_line + DISTANCE_LINE_OFFSET) )
    {
        return true;
    }
    return false;
}

bool IsCoordinateRow( unsigned line_num, unsigned last_header_line )
{
    if ( line_num == (last_header_line + COORDINATE_LINE_OFFSET) )
    {
        return true;
    }
    return false;
}

Movement MetersMoved( string line )
{
    stringstream line_ss;
    line_ss << line;
    string tmp;
    double val;
    vector<double> values;
    while ( !line_ss.eof() )
    {
        line_ss >> tmp;
        if ( stringstream(tmp) >> val )
        {
            values.push_back(val);
        }
    }

    Movement moved( values.at(2), values.at(3) );
    return moved;
}

// validated with https://gps-coordinates.org/distance-between-coordinates.php
// it was pretty dang close
Coordinates CalculateCoordinates( Coordinates initial_coordinates, Movement meters_moved )
{
    Coordinates new_coordinates;
    new_coordinates.lat = initial_coordinates.lat + 
                (180.0/M_PI) * ( meters_moved.north / R_EARTH );

    new_coordinates.lon = initial_coordinates.lon + 
                (180.0/M_PI) * ( meters_moved.east / (R_EARTH * cos(initial_coordinates.lat * M_PI/180)) );

    return new_coordinates;
}

string ReplaceCoordinates( string line, Coordinates new_coordinates)
{
    stringstream line_ss;
    line_ss << line;
    string tmp;
    double val;
    vector<string> values;
    string output_string = "";
    
    for (int element = 0; element < 5; element ++ )
    {
        // !line_ss.eof()
        line_ss >> tmp;
        if ( stringstream(tmp) >> val )
        {
            switch(element)
            {
            case 0:
                output_string.append( to_string( new_coordinates.lat ) );
                break;
            case 1:
                output_string.append(" ");
                output_string.append( to_string( new_coordinates.lon ) );
                break;
            default:
                output_string.append(" ");
                output_string.append(tmp);
                break;
            }
        }
    }
    // output_string.append("\r\n");

    return output_string;
}

// read each line, modify it, and write it to the output file
void do_something( string in_filename, string out_filename, Coordinates initial_coordinates )
{
    ifstream infile( in_filename );
    ofstream outfile( out_filename, std::ios_base::app);

    if( infile.is_open() )
    {
        string in_line;
        unsigned line_num = 0;
        unsigned latest_header_line = 0;
        bool was_data_row = false;
        Movement meters_moved(0,0);
        Coordinates new_coordinates(0,0);
        while ( getline( infile, in_line ) )
        {
            string out_line(in_line);
            if( IsHeaderRow( IsDataRow( in_line ), was_data_row ) )
            {
                // printf("header line %d \n", line_num);
                latest_header_line = line_num;
            }
            else if( IsDistanceRow( line_num, latest_header_line ) )
            {
                meters_moved = MetersMoved( in_line );
            }
            else if( IsCoordinateRow( line_num, latest_header_line ) )
            {
                new_coordinates = CalculateCoordinates( initial_coordinates, meters_moved );
                out_line = ReplaceCoordinates( in_line, new_coordinates);
            }

            out_line.append("\n");
            outfile << out_line;
            outfile.flush();

            was_data_row = IsDataRow( in_line );
            line_num++;
        }

        infile.close();
        line_num++;
    }
    return;
}

void print_help()
{
    printf("Usage: gps_converter INPUT OUTPUT LATITUDE LONGITUDE\n");
    printf("Calculate GPS coordinates given initial geographic location and distance\n");
    printf(" traveled from the INPUT file.\n");
    printf("The OUTPUT file is the INPUT file with updated coordinates.\n\n");
    printf("EXAMPLE: gps_converter InFile.txt OutFile.txt 42.029006 -93.628679\n");
    printf("\n");
    printf("  --help                  show this help\n");

}

} // namespace

int main(int argc, char *argv[])
{
    if( (argc == 2) && !strcmp(argv[1], "--help") )
    {
        print_help();
        return 0;
    }
    else if( argc != 5 )
    {
        printf("ERROR: Incorrect number of inputs!\n\n");
        print_help();
        return 0;
    }

    printf("Running...\n");

    string i_infile = argv[1];
    string i_outfile = argv[2];
    string i_lat = argv[3];
    string i_lon = argv[4];

    Coordinates initial_coordinates(stod(i_lat), stod(i_lon));
    
    do_something(i_infile, i_outfile, initial_coordinates);
    return 0;
}