/*
 * cservice pending_traffic table migration tool
 *
 * Example to import from database:
 * /usr/local/pgsql/bin/pg_dump --table=pending_traffic --data-only --column-inserts cservice > traffic.sql
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int atoi( const std::string& val )
{
	return ::atoi( val.c_str() ) ;
}

string GetNumericIP(int IP)
{
	std::stringstream s;
	s	<< static_cast< int >( (IP >> 24) & 0xff ) << '.'
		<< static_cast< int >( (IP >> 16) & 0xff ) << '.'
		<< static_cast< int >( (IP >>  8) & 0xff ) << '.'
		<< static_cast< int >( (IP & 0xff) ) ;
	return s.str() ;
}

int main(int argc, char* argv[])
{
	string progname = argv[0];
	string input_file;
	string output_file;
	int ip = 0;
	/* C style
	const char* pname = progname.c_str();
	if (pname[0] == '.') ++pname;
	if (pname[0] == '/') ++pname;
	progname = pname;
	*/
	// C++ style
	if (progname[0] == '.') progname = progname.substr(1, progname.length() - 1);
	if (progname[0] == '/') progname = progname.substr(1, progname.length() - 1);

	if (argc < 3) 
	{ // We expect 3 arguments: the program name, the source path and the destination path
		std::cerr << "Usage: " <<  progname << " <sourcefile> <destinationfile>" << std::endl;
		return 1;
	}
	input_file  = argv[1];
	output_file = argv[2];
	string line;
	std::cout << input_file << " <> " << output_file << endl;
	ifstream ifile(input_file.c_str());
	if (!ifile.is_open())
	{
		cout << "Unable to open " << input_file << endl;
		return 1;
	}
        ofstream ofile(output_file.c_str());
        if (!ofile.is_open())
        {
                cout << "Unable to open " << output_file << endl;
                return 1;
        }

	while (getline(ifile,line))
	{
		if (line.find("VALUES") != string::npos)
		{
			string backstr;
			backstr = line.substr(0, line.find("VALUES"));
			line = line.substr(line.find("VALUES"),line.length());
			backstr += line.substr(0, line.find(", ") + 2);
			line = line.substr(line.find(", ") + 2,line.length());
			string strip = line.substr(0, line.find(","));
			ip = atoi(strip);
			strip = GetNumericIP(ip);
			backstr += "'" + strip + "'" + line.substr(line.find(", "), line.length());
			cout << "ip = '" << strip << "'" << endl;
			cout  << backstr << '\n';
			ofile << backstr << '\n';
		}
		else
		{
                        //cout  << "'" << line << "'" << '\n';
                        ofile << line << '\n';
		}
	}
	ofile.close();
	ifile.close();
	return 0;
}
