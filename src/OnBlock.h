/*
 TinyCAD-OnBlock is schematic capture for the additional purpose of using TinyCAD as a Function Block Editor.
 TinyCAD sources are modified by countrymaker 2021.
 This program keeps on GNU LGPL.
 The modified lines are marked with "// cmaker"
*/

#pragma once

using namespace std;
using namespace rapidxml;

class COnBlock {
public:
	enum class compose_option {
		exclude_unconnected_parts,
		include_unconnected_parts
	};

private:
	struct part {
		string refname;
		string symname;
		vector<string> attname;
		vector<string> attinit;
		size_t attlen = 0;
	};

	struct hpart {
		string refname;
		string filename;
		vector<string> attname;
		vector<string> attinit;
		size_t attlen = 0;
	};

	struct pin {
		string refname;
		string pinname;
		string pinnum;
		string pintype;
	};

	struct net {
		string netname;
		string netnum;
		vector<pin> node;
		size_t netlen = 0;
	};

	struct Symbol {
		string symname;
		string refname;
		string pack;
		string type;
		vector<string> varname;
		vector<string> pinname;
		vector<string> pinnumb;
		vector<string> pintype;
		string codetext;
		string prolog;
		size_t varlen = 0;
		size_t pinlen = 0;
		bool isfb = false; // if is FB
		bool ncout = false; // if not connected output
		bool usetemp = false; // using template symbol class
	};

	struct Part {
		string refname;
		string symname;
		string pack;
		string type;
		string seq;
		vector<string> varname;
		vector<string> varinit;
		size_t varlen = 0;
		size_t seqnum = 0;
		size_t nestnum = 0;
		bool isfb = false;
		bool ncout = false;
		bool uni = false;
		bool usetemp = false;
	};

	struct Hpart {
		string refname;
		string filename;
		string pack;
		string type;
		string seq;
		vector<string> varname;
		vector<string> varinit;
		size_t varlen = 0;
		size_t seqnum = 0;
		size_t nestnum = 0;
	};

	struct Net {
		string netname;
		string netnum;
		vector<string> refname;
		vector<string> pinname;
		vector<string> pinnum;
		vector<string> pintype;
		size_t netlinklen = 0;
		vector<bool> isfb;
		vector<bool> ncout;
	};

	struct seqList {
		string refname;
		string symname;
		string pack;
		size_t seqnum = 0;
		bool ncout = false;
		bool usetemp = false;
	};

	string m_dsnfilename = "";
	string m_dsnname = "";
	string m_dsnfoldername = "";
	string m_netfilename = "";
	string m_inofilename = "";
	string m_corerefname = "";
	bool m_optimize = true;

	xml_document<char> m_xmldoc;
	string m_xmlbuf;
	vector<part> parts;
	vector<hpart> hparts;
	vector<net> nets;
	vector<Symbol> Symbols;
	vector<Part> Parts;
	vector<Hpart> Hparts;
	vector<Net> Nets;
	vector<seqList> SeqList;
	vector<string> m_incline;
	vector<string> m_errmsgs;
	size_t SymbolsLenOpt = 0;
	size_t PartsLenOpt = 0;
	size_t m_maxnest = 0;

private:
	bool isFB(const string& name);
	size_t getNestNum(const string& refname) noexcept;
	string getMotherRefname(const string& refname);
	string getType(const string& refname);
	string getSeq(const string& refname);
	void buildPkgNSeq();
	void buildSeqList();
	string getSeqOfNest(const string seqstr, size_t nest);
	signed cmpSeq(const string& s1, const string& s2);
	string buildNet(const string& refname);
	string name2type(const string& name);
	string name2name(const string& name);
	void getValidParts();
	void getValidSymbols();
	void ifTemplate();

public:
	size_t loadXmlFile(const string& xmlfile);
	void parse1();
	void parse2();
	string generateKicadNetlist();
	string composeInoFile(compose_option option);
	string getDsnName();
	string getDsnFolderName();
	string getNetFileName();
	string getInoFileName();
};

#define WHITE_SPACE " \t\n\r\f\v"
string rtrim(const string& s, const char* t = WHITE_SPACE);
string ltrim(const string& s, const char* t = WHITE_SPACE);
string trim(const string& s, const char* t = WHITE_SPACE);
string remove_char(const string& s, const char* t = WHITE_SPACE);
string first_substr(const string& s, const char* t = WHITE_SPACE);
string second_substr(const string& s, const char* t = WHITE_SPACE);
bool make_dir(const string& dirname);
string get_date_time();
