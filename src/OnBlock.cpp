/*
 TinyCAD-OnBlock is schematic capture for the purpose of using TinyCAD as a Function Block Editor.
 TinyCAD sources are modified by countrymaker 2021.
 This program keeps on GNU LGPL.
 The modified lines are marked with "// cmaker"
*/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "rapidxml-1.13/rapidxml.hpp"
#include "OnBlock.h"

// ------------------------------------------------------------------------------
// Private Methods
// ------------------------------------------------------------------------------

bool COnBlock::isFB(const string& name) {
	if (name.substr(0, 2) == "FB") return true;
	return false;
}

size_t COnBlock::getNestNum(const string& refname) noexcept {
	size_t n, i;

	for (n = 0, i = 0; i < refname.length(); i++) {
		n = refname.find("_", n + 1);
		if (n == string::npos) break;
	}
	return i;
}

string COnBlock::getMotherRefname(const string& refname) {
	size_t n;
	string str = refname;

	if ((n = str.find("_")) != string::npos) str.erase(0, n + 1);
	return str;
}

string COnBlock::getType(const string& refname) {
	for (size_t i = 0; i < Hparts.size(); i++) if (Hparts.at(i).refname == refname) return(Hparts.at(i).type);
	return "";
}

string COnBlock::getSeq(const string& refname) {
	for (size_t i = 0; i < Hparts.size(); i++) if (Hparts.at(i).refname == refname) return(Hparts.at(i).seq);
	return "";
}

void COnBlock::buildPkgNSeq() {
	size_t i, k, nest = 0;

	for (i = 0; i < Parts.size(); i++) {
		if (getNestNum(Parts.at(i).refname) > nest) nest = getNestNum(Parts.at(i).refname);
	}
	m_maxnest = nest;

	for (i = 0; i < Hparts.size(); i++) Hparts.at(i).nestnum = getNestNum(Hparts.at(i).refname);

	for (i = 0; i < Parts.size(); i++) Parts.at(i).nestnum = getNestNum(Parts.at(i).refname);

	for (k = 1; k <= m_maxnest; k++) {
		for (i = 0; i < Hparts.size(); i++) {
			if (Hparts.at(i).nestnum == k) {
				string refname = getMotherRefname(Hparts.at(i).refname);
				Hparts.at(i).type = getType(refname);
				Hparts.at(i).seq = getSeq(refname) + "." + Hparts.at(i).seq;
			}
		}
	}

	for (k = 1; k <= m_maxnest; k++) {
		for (i = 0; i < Parts.size(); i++) {
			if (Parts.at(i).nestnum == k) {
				string refname = getMotherRefname(Parts.at(i).refname);
				Parts.at(i).type = getType(refname);
				Parts.at(i).seq = getSeq(refname) + "." + Parts.at(i).seq;
			}
		}
	}
}

string COnBlock::getSeqOfNest(const string seqstr, size_t nest) {
	string str = seqstr;
	size_t n, dotcnt = 0;

	if (nest == 0) return str.substr(0, str.find("."));
	for (size_t i = 0; i < nest; i++) {
		n = str.find(".");
		if (n != string::npos) {
			str.erase(0, n + 1);
			dotcnt++;
			if (dotcnt == nest) return str.substr(0, str.find("."));
		}
		else return "";
	}
	return "";
}

signed COnBlock::cmpSeq(const string& s1, const string& s2) {
	for (size_t nest = 0; nest <= m_maxnest; nest++) {
		if ((getSeqOfNest(s1, nest) == "") && (getSeqOfNest(s2, nest) == "")) break;

		size_t n1, n2;
		if (getSeqOfNest(s1, nest) == "") n1 = 0;
		else n1 = stoi(getSeqOfNest(s1, nest));
		if (getSeqOfNest(s2, nest) == "") n2 = 0;
		else n2 = stoi(getSeqOfNest(s2, nest));

		if (n1 < n2) return -1;
		if (n1 > n2) return 1;
	}
	return 0;
}

void COnBlock::buildSeqList() {
	for (size_t i = 0; i < Parts.size(); i++) {
		size_t seqnum = 0;
		for (size_t j = 0; j < Parts.size(); j++) {
			if (cmpSeq(Parts.at(i).seq, Parts.at(j).seq) > 0) seqnum++;
		}
		Parts.at(i).seqnum = seqnum;
	}

	for (size_t seqnum = 0; seqnum < Parts.size(); seqnum++) {
		seqList aList;
		for (size_t i = 0; i < Parts.size(); i++) {
			if (!Parts.at(i).isfb) continue;
			if (Parts.at(i).seqnum == seqnum) {
				aList.refname = Parts.at(i).refname;
				aList.symname = Parts.at(i).symname;
				aList.pack = Parts.at(i).pack;
				aList.seqnum = seqnum;
				aList.ncout = Parts.at(i).ncout;
				aList.usetemp = Parts.at(i).usetemp;
				SeqList.push_back(aList);
			}
		}
	}
}

string COnBlock::buildNet(const string& refname) {
	string rstr = "";
	size_t i, j, k;

	for (i = 0; i < Nets.size(); i++) {
		for (j = 0; j < Nets.at(i).netlinklen; j++) {
			if (Nets.at(i).refname.at(j) == refname) {
				if (Nets.at(i).pintype.at(j) == "Output") {
					for (k = 0; k < Nets.at(i).netlinklen; k++) {
						if (Nets.at(i).pintype.at(k) != "Output") {
							if (!isFB(Nets.at(i).refname.at(k))) continue;
							if (!isFB(Nets.at(i).refname.at(j))) continue;
							if (!m_optimize || !Nets.at(i).ncout.at(k)) {
								rstr += "\t" + Nets.at(i).refname.at(k) + '.' + name2name(Nets.at(i).pinname.at(k))
									+ " = " + Nets.at(i).refname.at(j) + '.' + name2name(Nets.at(i).pinname.at(j))
									+ ";\n";
							}
						}
					}
				}
			}
		}
	}
	return rstr;
}

string COnBlock::name2type(const string& name) {
	string str = name;

	str = trim(str);
	if (second_substr(str).empty()) return "T";
	else return first_substr(str);
}

string COnBlock::name2name(const string& name) {
	string str = name;

	str = str.substr(0, str.find("="));
	str = trim(str);
	if (second_substr(str).empty()) return str;
	else return second_substr(str);
}

void COnBlock::getValidParts() {
	size_t i, j, k, m;
	size_t outnum, linknum;
	bool ncout, save;
	string pref, nref;

	for (m = 0; m < Parts.size(); m++) {
		for (i = 0; i < Parts.size(); i++) {
			pref = Parts.at(i).refname;
			for (ncout = false, save = false, j = 0; j < Nets.size(); j++) {
				for (linknum = 0, k = 0; k < Nets.at(j).netlinklen; k++) {
					if (Nets.at(j).ncout.at(k) == false) linknum++;
				}
				for (outnum = 0, k = 0; k < Nets.at(j).netlinklen; k++) {
					if (Nets.at(j).refname.at(k) == pref) {
						if (Nets.at(j).pintype.at(k) == "Output" && Nets.at(j).ncout.at(k) == false) outnum++;
					}
				}
				if (linknum == 1 && outnum == 1) ncout = true;
				else if (linknum > 1 && outnum == 1) save = 1;
			}
			if (ncout && !save) Parts.at(i).ncout = true;
		}

		for (i = 0; i < Parts.size(); i++) {
			pref = Parts.at(i).refname;
			if (Parts.at(i).ncout) {
				for (j = 0; j < Nets.size(); j++) {
					for (k = 0; k < Nets.at(j).netlinklen; k++) {
						nref = Nets.at(j).refname.at(k);
						if (nref == pref) Nets.at(j).ncout.at(k) = true;
					}
				}
			}
		}
	}

	for (j = 0, i = 0; i < Parts.size(); i++) {
		if (Parts.at(i).ncout == false) if (Parts.at(i).isfb) j++;
	}
	PartsLenOpt = j;
}

void COnBlock::getValidSymbols() {
	for (size_t i = 0; i < Symbols.size(); i++) {
		Symbols.at(i).ncout = true;
		for (size_t j = 0; j < Parts.size(); j++) {
			if (Parts.at(j).symname == Symbols.at(i).symname) if (Parts.at(j).ncout == false) Symbols.at(i).ncout = false;
		}
	}

	size_t j = 0;
	for (size_t i = 0; i < Symbols.size(); i++) {
		if ((Symbols.at(i).ncout == false) && (Symbols.at(i).isfb)) j++;
	}
	SymbolsLenOpt = j;
}

void COnBlock::ifTemplate() {
	for (size_t i = 0; i < Parts.size(); i++) {
		size_t typecnt = 0;
		for (size_t j = 0; j < Parts.size(); j++) {
			if (Parts.at(i).symname == Parts.at(j).symname) {
				if (Parts.at(i).type != Parts.at(j).type) typecnt++;
			}
		}
		if (typecnt) Parts.at(i).usetemp = true;
		else Parts.at(i).usetemp = false;
	}

	for (size_t i = 0; i < Symbols.size(); i++) {
		for (size_t j = 0; j < Parts.size(); j++) {
			if (Symbols.at(i).symname == Parts.at(j).symname) {
				Symbols.at(i).usetemp = Parts.at(j).usetemp;
				break;
			}
		}
	}
}

// ------------------------------------------------------------------------------
// Public Methods
// ------------------------------------------------------------------------------

size_t COnBlock::loadXmlFile(const string& filename) {
	char c, oldc = ' ';
	ifstream xmlfile(filename);

	while (xmlfile.get(c)) {
		if ((c == 0x0d) && (oldc == 0x0d));
		else m_xmlbuf.push_back(c);
		oldc = c;
	}
	m_xmlbuf.push_back(0x00);
	xmlfile.close();

	m_errmsgs.clear();

	return m_xmlbuf.size();
}

void COnBlock::parse1() {
	string name;
	string value;
	xml_node<char>* root;
	xml_node<char>* item;
	xml_node<char>* subitem;
	xml_node<char>* ssubitem;
	xml_attribute<>* attr;

	cout << "[parse1] ...............................\n";

	// ------------------
	// loading parts data
	// ------------------
	m_xmldoc.parse<0>(&m_xmlbuf.front());
	root = m_xmldoc.first_node();

	for (attr = root->first_attribute(); attr; attr = attr->next_attribute()) {
		if (string(attr->name()) == "design") {
			m_dsnfilename = attr->value();
			string stmp = m_dsnfilename;
			size_t n = stmp.find(".dsn");
			if (n != string::npos) {
				stmp = stmp.substr(0, n);
				m_dsnfoldername = stmp;
				n = stmp.find_last_of('\\');
				stmp.erase(0, n + 1);
				m_dsnname = stmp;
				m_netfilename = m_dsnfoldername + "\\" + m_dsnname + ".net";
				m_inofilename = m_dsnfoldername + "\\" + m_dsnname + ".ino";
			}
		}
	}

	for (item = root->first_node(); item; item = item->next_sibling()) {
		if (string(item->name()) == "parts") {
			for (subitem = item->first_node(); subitem; subitem = subitem->next_sibling()) {
				if (string(subitem->name()) == "part") {
					part a_part;
					for (attr = subitem->first_attribute(); attr; attr = attr->next_attribute()) {
						if (string(attr->name()) == "ref") a_part.refname = attr->value();
						if (string(attr->name()) == "name") a_part.symname = attr->value();
					}

					for (ssubitem = subitem->first_node(); ssubitem; ssubitem = ssubitem->next_sibling()) {
						if (string(ssubitem->name()) == "attribute") {
							for (attr = ssubitem->first_attribute(); attr; attr = attr->next_attribute()) {
								if ((string(attr->name()) == "name") && string(attr->value()).size()) {
									a_part.attname.push_back(attr->value());
									a_part.attinit.push_back(ssubitem->value());
									a_part.attlen++;
								}
							}
						}
					}
					parts.push_back(a_part);
				}
			}
		}
	}

	for (item = root->first_node(); item; item = item->next_sibling()) {
		if (string(item->name()) == "parts") {
			for (subitem = item->first_node(); subitem; subitem = subitem->next_sibling()) {
				if (string(subitem->name()) == "hierarchical-part") {
					hpart a_hpart;
					for (attr = subitem->first_attribute(); attr; attr = attr->next_attribute()) {
						if (string(attr->name()) == "ref") a_hpart.refname = attr->value();
						if (string(attr->name()) == "name") a_hpart.filename = attr->value();
					}

					for (ssubitem = subitem->first_node(); ssubitem; ssubitem = ssubitem->next_sibling()) {
						if (string(ssubitem->name()) == "attribute") {
							for (attr = ssubitem->first_attribute(); attr; attr = attr->next_attribute()) {
								if ((string(attr->name()) == "name") && string(attr->value()).size()) {
									a_hpart.attname.push_back(attr->value());
									a_hpart.attinit.push_back(ssubitem->value());
									a_hpart.attlen++;
								}
							}
						}
					}
					hparts.push_back(a_hpart);
				}
			}
		}
	}

	for (item = root->first_node(); item; item = item->next_sibling()) {
		if (string(item->name()) == "nets") {
			for (subitem = item->first_node(); subitem; subitem = subitem->next_sibling()) {
				if (string(subitem->name()) == "net") {
					net a_net;
					for (attr = subitem->first_attribute(); attr; attr = attr->next_attribute()) {
						if (string(attr->name()) == "name") a_net.netname = attr->value();
						if (string(attr->name()) == "number") a_net.netnum = attr->value();
					}

					for (ssubitem = subitem->first_node(); ssubitem; ssubitem = ssubitem->next_sibling()) {
						if (string(ssubitem->name()) == "pin") {
							pin a_pin;
							for (attr = ssubitem->first_attribute(); attr; attr = attr->next_attribute()) {
								if ((string(attr->name()) == "part") && string(attr->value()).size()) {
									a_pin.refname = attr->value();
								}
								if ((string(attr->name()) == "name") && string(attr->value()).size()) {
									a_pin.pinname = attr->value();
								}
								if ((string(attr->name()) == "number") && string(attr->value()).size()) {
									a_pin.pinnum = attr->value();
								}
								if ((string(attr->name()) == "type") && string(attr->value()).size()) {
									a_pin.pintype = attr->value();
								}
							}
							a_net.node.push_back(a_pin);

							a_net.netlen++;
						}
					}
					nets.push_back(a_net);
				}
			}
		}
	}

	cout << "partslen = " << parts.size() << endl;
	cout << "hpartslen = " << hparts.size() << endl;
	cout << "netslen = " << nets.size() << endl << endl;
}

void COnBlock::parse2() {
	size_t i, j, k, m;
	size_t be, varlen, pinlen;
	bool err = false;
	string errstr = "";

	cout << "[parse2] ...............................\n";

	// --------------------
	// loading Symbols data
	// --------------------
	for (i = 0; i < parts.size(); i++) { // checking duplicate symbol names
		for (be = 0, j = 0; j < Symbols.size(); j++) {
			if (parts.at(i).symname == Symbols.at(j).symname) be++;
		}
		if (be == 0) {
			Symbol aSymbol;

			aSymbol.symname = parts.at(i).symname;
			aSymbol.refname = parts.at(i).refname;
			for (k = 0, varlen = 0; k < parts.at(i).attlen; k++) {
				if (parts.at(i).attname.at(k) == "$$SPICE") aSymbol.codetext = parts.at(i).attinit.at(k);
				else if (parts.at(i).attname.at(k) == "$$SPICE_PROLOG_PRIORITY");
				else if (parts.at(i).attname.at(k) == "$$SPICE_EPILOG_PRIORITY");
				else if (parts.at(i).attname.at(k) == "$$SPICE_PROLOG") aSymbol.prolog = parts.at(i).attinit.at(k);
				else if (parts.at(i).attname.at(k) == "$$SPICE_EPILOG");
				else if (parts.at(i).attname.at(k) == "Package") aSymbol.pack = parts.at(i).attinit.at(k);
				else if (parts.at(i).attname.at(k) == "Type") aSymbol.type = parts.at(i).attinit.at(k);
				else if (parts.at(i).attname.at(k) == "Seq");
				else if (parts.at(i).attname.at(k) == "Uni");
				else {
					aSymbol.varname.push_back(parts.at(i).attname.at(k));
					varlen++;
				}
			}
			aSymbol.varlen = varlen;
			Symbols.push_back(aSymbol);
		}
	}

	for (i = 0; i < Symbols.size(); i++) { // loading Symbols data
		for (j = 0; j < parts.size(); j++) {
			if (parts.at(j).symname == Symbols.at(i).symname) {
				pinlen = 0;
				for (k = 0; k < nets.size(); k++) {
					for (m = 0; m < nets.at(k).netlen; m++) {
						if (Symbols.at(i).refname == nets.at(k).node.at(m).refname) {
							Symbols.at(i).pinname.push_back(nets.at(k).node.at(m).pinname);
							Symbols.at(i).pinnumb.push_back(nets.at(k).node.at(m).pinnum);
							Symbols.at(i).pintype.push_back(nets.at(k).node.at(m).pintype);
							pinlen++;
						}
					}
				}
				Symbols.at(i).pinlen = pinlen;
			}
		}
		Symbols.at(i).ncout = false;
	}

	// --------------------------------------------
	// Searching "#include" & loading Symbols codes
	// --------------------------------------------
	m_incline.clear();
	for (i = 0; i < Symbols.size(); i++) {
		string codetext;
		string line;
		string ncode = "";
		string istr = Symbols.at(i).codetext;

		while (istr.size() > 0) {
			const size_t n = istr.find("\n");
			if (n == string::npos) {
				line = istr;
				istr.clear();
			}
			else line = istr.substr(0, n - 1);

			if (line.find("#include ") != string::npos) {
				line.erase(0, 9);
				for (j = 0; j < m_incline.size(); j++) if (line == m_incline.at(j)) break;
				if (j == m_incline.size()) m_incline.push_back(line);
			}
			else ncode += "\t" + line + "\n";
			istr = istr.substr(n + 1);
		}
		Symbols.at(i).codetext = ncode;
	}

	// ------------------
	// loading Parts data
	// ------------------
	for (i = 0; i < parts.size(); i++) {
		Part aPart;
		aPart.refname = parts.at(i).refname;
		aPart.symname = parts.at(i).symname;

		aPart.varlen = 0;

		for (j = 0; j < parts.at(i).attlen; j++) {
			if (parts.at(i).attname.at(j) == "$$SPICE");
			else if (parts.at(i).attname.at(j) == "$$SPICE_PROLOG_PRIORITY");
			else if (parts.at(i).attname.at(j) == "$$SPICE_EPILOG_PRIORITY");
			else if (parts.at(i).attname.at(j) == "$$SPICE_PROLOG");
			else if (parts.at(i).attname.at(j) == "$$SPICE_EPILOG");
			else if (parts.at(i).attname.at(j) == "Package") aPart.pack = parts.at(i).attinit.at(j);
			else if (parts.at(i).attname.at(j) == "Type") aPart.type = parts.at(i).attinit.at(j);
			else if (parts.at(i).attname.at(j) == "Seq") {
				aPart.seq = parts.at(i).attinit.at(j);
				aPart.seqnum = stoi(aPart.seq);
			}
			else if (parts.at(i).attname.at(j) == "Uni") aPart.uni = true;
			else {
				aPart.varname.push_back(parts.at(i).attname.at(j));
				aPart.varinit.push_back(parts.at(i).attinit.at(j));
				aPart.varlen++;
			}
		}

		aPart.nestnum = 0;
		aPart.ncout = false;

		Parts.push_back(aPart);
	}

	for (i = 0; i < Parts.size(); i++) Parts.at(i).isfb = isFB(Parts.at(i).refname);

	for (i = 0; i < Symbols.size(); i++) {
		for (j = 0; j < Parts.size(); j++) {
			if (Symbols.at(i).symname == Parts.at(j).symname) Symbols.at(i).isfb = Parts.at(j).isfb;
		}
	}

	for (err = false, i = 0; i < Parts.size(); i++) {	// Checking duplicate ref name
		for (j = 0; j < Parts.size(); j++) {
			if ((i != j) && (Parts.at(i).refname == Parts.at(j).refname)) {
				err = true;
				errstr = "duplicate reference name -> Regenerate FB symbol references.";
				m_errmsgs.push_back(errstr);
				break;
			}
		}
		if (err) break;
	}

	for (err = false, i = 0; i < Hparts.size(); i++) {
		for (j = 0; j < Hparts.size(); j++) {
			if ((i != j) && (Hparts.at(i).refname == Hparts.at(j).refname)) {
				err = true;
				errstr = "duplicate reference name -> Regenerate FB symbol references.";
				m_errmsgs.push_back(errstr);
				break;
			}
		}
		if (err) break;
	}

	for (i = 0; i < Parts.size(); i++) {	// Checking no cores
		if (Parts.at(i).pack == "core") break;
	}
	if (i == Parts.size()) {
		errstr = "core FB not found";
		m_errmsgs.push_back(errstr);
	}

	for (err = false, i = 0; i < Parts.size(); i++) {	// Checking duplicate cores
		if (Parts.at(i).pack != "core") continue;
		m_corerefname = Parts.at(i).refname;
		for (j = 0; j < Parts.size(); j++) {
			if ((i != j) && (Parts.at(i).pack == Parts.at(j).pack)) {
				err = true;
				errstr = "duplicate cores -> A design has only one core part.";
				m_errmsgs.push_back(errstr);
				break;
			}
		}
		if (err) break;
	}

	if (Parts.size()) {	// Checking duplicate uni-parts
		for (err = false, i = 0; i < Parts.size(); i++) {
			if (!Parts.at(i).uni) continue;
			for (j = 0; j < Parts.size(); j++) {
				if ((i != j) && (Parts.at(i).uni == Parts.at(j).uni)) {
					err = true;
					errstr = "duplicate uni-parts -> A design can't exceed one uni-part FB with the same FB symbol.";
					m_errmsgs.push_back(errstr);
					break;
				}
			}
			if (err) break;
		}
	}

	// -------------------
	// loading Hparts data
	// -------------------
	for (i = 0; i < hparts.size(); i++) {
		Hpart aHpart;
		aHpart.refname = hparts.at(i).refname;
		aHpart.filename = hparts.at(i).filename;

		aHpart.varlen = 0;
		for (j = 0; j < hparts.at(i).attlen; j++) {
			if (hparts.at(i).attname.at(j) == "$$SPICE");
			else if (hparts.at(i).attname.at(j) == "$$SPICE_PROLOG_PRIORITY");
			else if (hparts.at(i).attname.at(j) == "$$SPICE_EPILOG_PRIORITY");
			else if (hparts.at(i).attname.at(j) == "Other");
			else if (hparts.at(i).attname.at(j) == "Package") aHpart.pack = hparts.at(i).attinit.at(j);
			else if (hparts.at(i).attname.at(j) == "Type") aHpart.type = hparts.at(i).attinit.at(j);
			else if (hparts.at(i).attname.at(j) == "Seq") {
				aHpart.seq = hparts.at(i).attinit.at(j);
				aHpart.seqnum = stoi(aHpart.seq);
			}
			else {
				aHpart.varname.push_back(hparts.at(i).attname.at(j));
				aHpart.varinit.push_back(hparts.at(i).attinit.at(j));
				aHpart.varlen++;
			}
		}
		aHpart.nestnum = 0;
		Hparts.push_back(aHpart);
	}

	// -----------------
	// loading Nets data
	// -----------------
	for (i = 0; i < nets.size(); i++) {
		Net aNet;
		aNet.netlinklen = nets.at(i).netlen;
		aNet.netname = nets.at(i).netname;
		aNet.netnum = nets.at(i).netnum;
		for (j = 0; j < aNet.netlinklen; j++) {
			aNet.refname.push_back(nets.at(i).node.at(j).refname);
			aNet.pinname.push_back(nets.at(i).node.at(j).pinname);
			aNet.pinnum.push_back(nets.at(i).node.at(j).pinnum);
			aNet.pintype.push_back(nets.at(i).node.at(j).pintype);

			aNet.ncout.push_back(false);
			aNet.isfb.push_back(isFB(aNet.refname.at(j)));
		}
		Nets.push_back(aNet);
	}

	getValidParts();
	getValidSymbols();
	buildPkgNSeq();
	ifTemplate();

	if (!m_errmsgs.empty()) {
		for (i = 0; i < m_errmsgs.size(); i++) cout << m_errmsgs.at(i) << endl;
		SymbolsLenOpt = PartsLenOpt = 0;
		m_incline.clear();
	}
	else cout << "no error\n";

	cout << "SymbolsLen = " << Symbols.size() << endl;
	cout << "PartsLen = " << Parts.size() << endl;
	cout << "HpartsLen = " << Hparts.size() << endl;
	cout << "NetsLen = " << Nets.size() << endl << endl;
}

string COnBlock::generateKicadNetlist() {
	string codetext = "";
	string netname;
	unsigned timestamp = 50000001;
	size_t i, j, isfb;

	codetext += "(export (version D)\n";
	codetext += "\t(components\n";
	for (i = 0; i < Parts.size(); i++) {
		if (Parts.at(i).isfb) continue;
		codetext += "\t\t(comp (ref " + Parts.at(i).refname += ")\n";
		codetext += "\t\t\t(value " + Parts.at(i).symname + ")\n";
		codetext += "\t\t\t(footprint " + Parts.at(i).pack; codetext += ")\n";
		codetext += "\t\t\t(tstamp " + to_string(timestamp++) + ")\n";
		codetext += "\t\t)\n";
	}
	codetext += "\t)\n";

	codetext += "\t(nets\n";
	for (i = 0; i < Nets.size(); i++) {
		isfb = 0;
		for (j = 0; j < Nets.at(i).netlinklen; j++) {
			if (Nets.at(i).isfb.at(j)) isfb++;
		}
		if (isfb) continue;
		if (Nets.at(i).netlinklen < 2) continue;

		codetext += "\t\t(net (code " + Nets.at(i).netnum + ")";
		netname = Nets.at(i).netname;
		if (netname.empty()) netname = "N" + Nets.at(i).netnum;
		codetext += " (name \"" + netname + "\")\n";
		for (j = 0; j < Nets.at(i).netlinklen; j++) {
			codetext += "\t\t\t(node (ref " + Nets.at(i).refname.at(j) + ")";
			codetext += " (pin " + Nets.at(i).pinnum.at(j) + "))\n";
		}
		codetext += "\t\t)\n";
	}
	codetext += "\t)\n";
	codetext += ")\n";

	return codetext;
}

string COnBlock::composeInoFile(compose_option option) {
	if (option == compose_option::exclude_unconnected_parts) m_optimize = true;
	else if (option == compose_option::include_unconnected_parts) m_optimize = false;
	else return("");

	string codetext = "";

	codetext += "// " + m_inofilename + " composed in TinyCad-OnBlock 2021\n";
	codetext += "// " + get_date_time() + " created.\n";

	size_t symbolslen, partslen;
	if (m_optimize) {
		symbolslen = SymbolsLenOpt;
		partslen = PartsLenOpt;
	}
	else {
		symbolslen = Symbols.size();
		partslen = Parts.size();
	}
	codetext += "// " + to_string(symbolslen) + " FB-classes generated.\n";
	codetext += "// " + to_string(partslen) + " FB-instances generated.\n";

	if (m_optimize) codetext += "// Unconnected FBs are ignored.\n\n";

	if (!m_errmsgs.empty()) {
		const size_t n = m_errmsgs.size();
		for (size_t i = 0; i < n; i++) codetext += "Error: " + m_errmsgs.at(i) + "\n";
		codetext += "\n";

		return codetext;
	}

	codetext += "#define DSN_NAME \"" + m_dsnname + "\"\n\n";

	if (const size_t n = m_incline.size()) {
		for (size_t i = 0; i < n; i++) {
			codetext += "#include " + m_incline.at(i) + "\n";
		}
		codetext += "\n";
	}

	for (size_t i = 0; i < Symbols.size(); i++) {
		if (!Symbols.at(i).prolog.empty()) {
			codetext += Symbols.at(i).prolog;
		}
	}

	codetext += "using namespace std;\n\n";
	codetext += "void loopFBs();\n\n";

	for (size_t i = 0; i < Symbols.size(); i++) {
		size_t len;
		if (m_optimize && Symbols.at(i).ncout) continue;
		if (!Symbols.at(i).isfb) continue;

		codetext += "// -----------------------------------------------------------------\n";
		codetext += "// FB Class: " + Symbols.at(i).symname + "\n";
		codetext += "// -----------------------------------------------------------------\n";
		if(Symbols.at(i).usetemp)
			codetext += "template<typename T> class " + Symbols.at(i).symname + " {\n";
		else
			codetext += "class " + Symbols.at(i).symname + " {\n";

		if (Symbols.at(i).varlen) {
			codetext += "private:\n";
			for (size_t j = 0; j < Symbols.at(i).varlen; j++) {	// Var members
				codetext += "\t";

				string type;
				if (!Symbols.at(i).usetemp && (name2type(Symbols.at(i).varname.at(j)) == "T")) type = Symbols.at(i).type;
				else type = name2type(Symbols.at(i).varname.at(j));
				codetext += type + " " + name2name(Symbols.at(i).varname.at(j)) + "; // Var\n";
			}
			codetext += "\n";
		}

		if (Symbols.at(i).pinlen == 0) codetext.pop_back();
		else codetext += "public:\n";

		for (size_t j = 0; j < Symbols.at(i).pinlen; j++) {	// Pin members
			codetext += "\t";

			string type;
			if (!Symbols.at(i).usetemp && (name2type(Symbols.at(i).pinname.at(j)) == "T")) type = Symbols.at(i).type;
			else type = name2type(Symbols.at(i).pinname.at(j));
			codetext += type + " " + name2name(Symbols.at(i).pinname.at(j)) + "; // ";
			codetext += Symbols.at(i).pintype.at(j) + "\n";
		}

		codetext += "\npublic:\n";	// Creator
		codetext += "\t" + Symbols.at(i).symname + "(";

		string line = "";
		for (size_t j = 0; j < Symbols.at(i).varlen; j++) {
			if (j > 0) line += ", ";

			string type;
			if (!Symbols.at(i).usetemp && (name2type(Symbols.at(i).varname.at(j)) == "T")) type = Symbols.at(i).type;
			else type = name2type(Symbols.at(i).varname.at(j));
			line += type + " " + name2name(Symbols.at(i).varname.at(j));
		}
		line += ')';
		codetext += line + "\n";

		len = 0;
		for (size_t j = 0; j < Symbols.at(i).varlen; j++) {	// Member Initialize
			if (len == 0) line = "\t\t: ";
			else line = ", ";
			line += name2name(Symbols.at(i).varname.at(j)) + "(" + name2name(Symbols.at(i).varname.at(j)) + ")";
			codetext += line;
			len++;
		}
		if (len == 0) codetext.erase(codetext.rfind("\n"), 1);
		codetext += " {}\n\n";

		codetext += Symbols.at(i).codetext + "\n";
		codetext.erase(codetext.length() - 2, codetext.length());
		codetext += "\n};\n\n";
	}

	codetext += "// -----------------------------------------------------------------\n";
	codetext += "// FB Instances \n";
	codetext += "// -----------------------------------------------------------------\n";

	for (size_t i = 0; i < Parts.size(); i++) {
		if (m_optimize && Parts.at(i).ncout) continue;
		if (!Parts.at(i).isfb) continue;

		string line;
		if (Parts.at(i).usetemp) line = Parts.at(i).symname + "<" + Parts.at(i).type + "> " + Parts.at(i).refname;
		else line = Parts.at(i).symname + " " + Parts.at(i).refname;

		bool ini = true;
		for (size_t j = 0; j < Parts.at(i).varlen; j++) {
			if (Parts.at(i).varname.at(j).find("(") != string::npos)  continue;
			if (ini) {
				line += "(";
				ini = false;
			}
			else line += ", ";

			if (Parts.at(i).varinit.at(j).empty()) line += "NULL";
			else line += Parts.at(i).varinit.at(j);
		}
		if (ini) line += ";";
		else line += ");";

		codetext += line + "\n";
	}

	buildSeqList();

	codetext += "\n";
	codetext += "// -----------------------------------------------------------------\n";
	codetext += "// Calling Sequences \n";
	codetext += "// -----------------------------------------------------------------\n";

	codetext += "void initFBs() {\n";
	for (size_t i = 0; i < SeqList.size(); i++) {
		if (m_optimize && SeqList.at(i).ncout) continue;
		codetext += "\t" + SeqList.at(i).refname + ".init();\n";
		if(SeqList.at(i).pack != "core") codetext += "\t" + SeqList.at(i).refname + ".exec();\n";
		codetext += buildNet(SeqList.at(i).refname) + "\n";
	}
	codetext.erase(codetext.length() - 1, 1);
	codetext += "}\n\n";

	codetext += "void loopFBs() {\n";
	for (size_t i = 0; i < SeqList.size(); i++) {
		if (SeqList.at(i).pack == "const") continue;
		if (m_optimize && SeqList.at(i).ncout) continue;
		codetext += "\t" + SeqList.at(i).refname + ".exec();\n";
		codetext += buildNet(SeqList.at(i).refname) + "\n";
	}
	codetext.erase(codetext.length() - 1, 1);
	codetext += "}\n\n";

	codetext += "// -----------------------------------------------------------------\n";
	codetext += "// Arduino setup() & loop() \n";
	codetext += "// -----------------------------------------------------------------\n";

	codetext += "void setup() {\n";
	codetext += "\tinitFBs();\n";
	codetext += "\t" + m_corerefname + ".start();\n";
	codetext += "}\n\n";
	codetext += "void loop() {\n";
	if (m_errmsgs.empty()) codetext += "\t" + m_corerefname + ".loop();\n";
	codetext += "}\n\n";

	/*for (size_t i = 0; i < Symbols.size(); i++) {
		if (false) {
			codetext += Symbols.at(i).codetext + "\n";
			break;
		}
	}*/
	codetext += "// End of " + m_inofilename + "\n";

	return codetext;
}

// ------------------------------------------------------------------------------
// Global Functions
// ------------------------------------------------------------------------------

string COnBlock::getDsnName() { return m_dsnname; }

string COnBlock::getDsnFolderName() { return m_dsnfoldername; }

string COnBlock::getNetFileName() { return m_netfilename; }

string COnBlock::getInoFileName() { return m_inofilename; }

string rtrim(const string& s, const char* t) {
	string str = s;

	str.erase(str.find_last_not_of(t) + 1);
	return str;
}

string ltrim(const string& s, const char* t) {
	string str = s;

	str.erase(0, str.find_first_not_of(t));
	return str;
}

string trim(const string& s, const char* t) {
	return ltrim(rtrim(s, t), t);
}

string remove_char(const string& s, const char* t) {
	string str = s;
	size_t n;

	while ((n = str.find_first_of(t)) != string::npos) str.erase(n, 1);
	return str;
}

string first_substr(const string& s, const char* t) {
	string str = s;

	str = ltrim(str);
	str.erase(str.find_first_of(t));
	return str;
}

string second_substr(const string& s, const char* t) {
	string str = s;

	str = ltrim(str);
	str.erase(0, str.find_first_of(t));
	str = ltrim(str);
	return str;
}

bool make_dir(const string& dirname) {
	CFileFind file;
	bool result = file.FindFile(CString(dirname.c_str()));

	if (!result) {
		result = CreateDirectory(CString(dirname.c_str()) + "\\", NULL);
		if (!result) return false;
		return true;
	}
	return false;
}

string get_date_time() {
	const auto now = chrono::system_clock::now();
	const auto in_time_t = chrono::system_clock::to_time_t(now);
	stringstream ss;
	ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}