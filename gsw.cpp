//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// gsw
// Copyright 2015 Gabor T. Marth, University of Utah
// All rights reserved.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// standard includes
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <cmath>


// "tclap" commandline parsing library
#include <tclap/CmdLine.h>

// "boost" regular expression library
#include <boost/regex.hpp>
#include <boost/format.hpp>

// include classes
#include "Class-GraphAlignment.h"
#include "Class-Traceback.h"
#include "Class-Pileup.h"
#include "Class-Graph.h"
#include "ArrayUtil.h"

//header only structs
#include "Variant.h"
#include "Coords.h"

// uses
using namespace std; 
using namespace TCLAP; 

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// templates
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// subroutines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

struct ArgStruct {
  string shortId;
  string longId;
  string description;
  bool required;
  string defaultValueString;
  int defaultValueInt;
  double defaultValueDouble;
  bool defaultValueBool;
  string type;
  bool multi;
  vector<string> constraint;
};

struct AlleleData {
  string base;
  short qual;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static string ProgramName("gsw");
static string ProgramDescription("Smith-Waterman optimal alignment algorithm");
static string ProgramVersion("0.4.1");
static string ProgramDate("2015-04-18");
static string ProgramDeveloper("Gabor T. Marth");
static string ProgramInstitution("University of Utah"); 
static string ProgramCopyrightDates("2015");

static vector<ArgStruct> ArgList;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// classes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


class MyOutput : public StdOutput {
public:
  
  virtual void failure(CmdLineInterface& c, ArgException& e)
  {
    cerr << "################################################################################" << endl;
    cerr << "### Program: " << ProgramName << " Version: " <<  ProgramVersion << " Release date: " << ProgramDate << endl;
    cerr << "### " << ProgramDescription << endl;
    cerr << "### " << "Copyright: " << ProgramCopyrightDates << " " << ProgramDeveloper << ", " << ProgramInstitution << endl;
    cerr << "### All rights reserved." << endl;
    cerr << "###" << endl;
    cerr << "### Command line error:" << e.what() << endl;
    cerr << "### For usage please type: " << c.getProgramName() << " --help" << endl;
    cerr << "################################################################################" << endl;
  }
  
  virtual void usage(CmdLineInterface& c)
  {

    cout << "################################################################################" << endl;
    cout << "### Program: " << ProgramName << " Version: " <<  ProgramVersion << " Release date: " << ProgramDate << endl;
    cout << "### " << ProgramDescription << endl;
    cout << "### " << "Copyright: " << ProgramCopyrightDates << " " << ProgramDeveloper << ", " << ProgramInstitution << endl;
    cout << "### All rights reserved." << endl;
    cout << "###" << endl;
    cout << "### Usage: " << c.getProgramName() << " [arguments], where:" << endl;
    for(vector<ArgStruct>::const_iterator it = ArgList.begin(); 
	it != ArgList.end(); it++) {
      ArgStruct arg = *it;

      string idString = "";
      if (arg.longId != "") {idString += "--" + arg.longId;}
      if (arg.shortId != "") {idString += ", -" + arg.shortId;}

      string multiString = "single-valued";
      if (arg.multi) {multiString = "multi-valued";}

      if (arg.required) {
	cout << "### " << idString << " [" << arg.type << ", required, no default, " << multiString << "]" << endl;
      }
      else {
	cout << "### " << idString << " [" << arg.type << ", optional, default=" << arg.defaultValueString << ", " << multiString << "]" << endl;
      }
      if (arg.constraint.size() > 0) {
	cout << "###     Permitted values: (";
	bool first = true;
	for (vector<string>::const_iterator iter = arg.constraint.begin();
	     iter != arg.constraint.end(); iter++) {
	  string value = *iter;
	  if (! first) {
	    cout << "|";
	  } 
	  first = false;
	  cout << value;
	}
	cout << ")" << endl;
      }
      cout << "###     Description: " << arg.description << endl;
    }
    cout << "################################################################################" << endl;
  }
  
  virtual void version(CmdLineInterface& c)
  {
    cerr << "################################################################################" << endl;
    cerr << "### Program: " << ProgramName << " Version: " <<  ProgramVersion << " Release date: " << ProgramDate << endl;
    cout << "### " << ProgramDescription << endl;
    cout << "### " << "Copyright: " << ProgramCopyrightDates << " " << ProgramDeveloper << ", " << ProgramInstitution << endl;
    cout << "### All rights reserved." << endl;
    cout << "###" << endl;
    cerr << "################################################################################" << endl;
  }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int main (int argc, char *argv[]) {

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // constants
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  static int D = 0;
  static int H = 1;
  static int V = 2;


  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // command line
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // Create new CmdLine object
  //----------------------------------------------------------------------------
  CmdLine cmd("", ' ', ProgramVersion);
    
  //----------------------------------------------------------------------------
  // add program-specific command line arguments
  //----------------------------------------------------------------------------

  // initialize arg
  ArgStruct arg;

  // subject
  ArgStruct argSubject;
  arg = argSubject; 
  arg.shortId = "s"; 
  arg.longId = "subject"; 
  arg.description = "Subject sequence";
  arg.required = false; 
  arg.defaultValueString = "AACCGGTT"; 
  arg.type = "string"; 
  arg.multi = false; 
  ArgList.push_back(arg);
  ValueArg<string> cmd_subject(arg.shortId, arg.longId, arg.description, arg.required, arg.defaultValueString, arg.type, cmd);

  // query
  ArgStruct argQuery;
  arg = argQuery; 
  arg.shortId = "q"; 
  arg.longId = "query"; 
  arg.description = "Query sequence";
  arg.required = false; 
  arg.defaultValueString = "GGAATTT"; 
  arg.type = "string"; 
  arg.multi = false; 
  ArgList.push_back(arg);
  ValueArg<string> cmd_query(arg.shortId, arg.longId, arg.description, arg.required, arg.defaultValueString, arg.type, cmd);

  // match score
  ArgStruct argM;
  arg = argM;
  arg.shortId = "";
  arg.longId = "M";
  arg.description = "Match score";
  arg.required = false;
  arg.defaultValueString = "2";
  arg.type = "int";
  arg.multi = false;
  ArgList.push_back(arg);
  ValueArg<int> cmd_M(arg.shortId, arg.longId, arg.description, arg.required, 2, arg.type, cmd);

  // mismatch score
  ArgStruct argX;
  arg = argX;
  arg.shortId = "";
  arg.longId = "X";
  arg.description = "Mismatch score";
  arg.required = false;
  arg.defaultValueString = "-2";
  arg.type = "int";
  arg.multi = false;
  ArgList.push_back(arg);
  ValueArg<int> cmd_X(arg.shortId, arg.longId, arg.description, arg.required, -2, arg.type, cmd);

  // gap initiation score
  ArgStruct argGI;
  arg = argX;
  arg.shortId = "";
  arg.longId = "GI";
  arg.description = "Gap initiation score";
  arg.required = false;
  arg.defaultValueString = "-3";
  arg.type = "int";
  arg.multi = false;
  ArgList.push_back(arg);
  ValueArg<int> cmd_GI(arg.shortId, arg.longId, arg.description, arg.required, -3, arg.type, cmd);

  // gap extension score
  ArgStruct argGE;
  arg = argX;
  arg.shortId = "";
  arg.longId = "GE";
  arg.description = "Gap extension score";
  arg.required = false;
  arg.defaultValueString = "-1";
  arg.type = "int";
  arg.multi = false;
  ArgList.push_back(arg);
  ValueArg<int> cmd_GE(arg.shortId, arg.longId, arg.description, arg.required, -1, arg.type, cmd);

  // print matrix?
  ArgStruct argMatrix;
  arg = argMatrix;
  arg.shortId = "";
  arg.longId = "matrix";
  arg.description = "Print score matrix?";
  arg.required = false;
  arg.defaultValueString = "false";
  arg.type = "switch";
  arg.multi = false;
  ArgList.push_back(arg);
  SwitchArg cmd_matrix(arg.shortId, arg.longId, arg.description, cmd, false);

  // debug
  ArgStruct argDebug;
  arg = argDebug;
  arg.shortId = "";
  arg.longId = "debug";
  arg.description = "Print debugging messages?";
  arg.required = false;
  arg.defaultValueString = "false";
  arg.type = "switch";
  arg.multi = false;
  ArgList.push_back(arg);
  SwitchArg cmd_debug(arg.shortId, arg.longId, arg.description, cmd, false);

  //----------------------------------------------------------------------------
  // register (but not add to cmd) special arguments that are automatically
  // added to cmd
  //----------------------------------------------------------------------------

  // help
  ArgStruct argHelp;
  arg = argHelp;
  arg.shortId = "h";
  arg.longId = "help";
  arg.description = "Print usage statement?";
  arg.required = false;
  arg.defaultValueString = "false";
  arg.type = "switch";
  arg.multi = false;
  ArgList.push_back(arg);

  // version
  ArgStruct argVersion;
  arg = argVersion;
  arg.shortId = "";
  arg.longId = "version";
  arg.description = "Print program version?";
  arg.required = false;
  arg.defaultValueString = "false";
  arg.type = "switch";
  arg.multi = false;
  ArgList.push_back(arg);

  //----------------------------------------------------------------------------
  // add custom output handler
  //----------------------------------------------------------------------------
  MyOutput my;
  cmd.setOutput(&my);

  //----------------------------------------------------------------------------
  // parse command line and catch possible errors
  //----------------------------------------------------------------------------
  try {
    cmd.parse(argc,argv);
  } 
  catch ( ArgException& e ) { 
    cerr << "ERROR: " << e.error() << " " << e.argId() << endl; 
  }
  
  //----------------------------------------------------------------------------
  // assign command line parameters
  //----------------------------------------------------------------------------

  string subject = cmd_subject.getValue();
  string query = cmd_query.getValue();
  int M = cmd_M.getValue();
  int X = cmd_X.getValue();
  int GI = cmd_GI.getValue();
  int GE = cmd_GE.getValue();
  bool matrix = cmd_matrix.getValue();
  bool debug = cmd_debug.getValue();

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // check and fix command line options
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // report command line and parameters used
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  map<bool, string, less<bool> > bool2String;
  bool2String[false] = "false";
  bool2String[true] = "true";

  if (debug) {
    cerr << "Command line:";
    for (int i=0; i<argc; i++) {
      cerr << " " << argv[i];
    }
    cerr << endl;
    cerr << endl;
    cerr << "Complete list of parameter values:" << endl;
    cerr << "  --subject = " << subject << endl;
    cerr << "  --query = " << query << endl;
    cerr << "  --M = " << M << endl;
    cerr << "  --X = " << X << endl;
    cerr << "  --GI = " << GI << endl;
    cerr << "  --GE = " << GE << endl;
    cerr << "  --matrix = " <<  bool2String[matrix] << endl;
    cerr << "  --debug = " <<  bool2String[debug] << endl;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // main code starts here
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  
 
  
  //Make a single graph from our reference sequence and SV 
  subject = "GAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGAGGTCTTGGACCTGTGGTCCTAATGCAAGATAAGGCCACGGGGCCTGAG";
  Variant v = {subject, make_pair("TTGCAAGAGGTCTTGGGACCTGTGGTCCTAA","T"), 184258399-184258374};

  //Variant v = {subject, make_pair("AGAGGTCTTGGGACCTGTGGTCCTAA","T"), 184258404-184258374};

  Graph g(v);

  string q1 = "CATCCACTTTAGCAAAGTGGAGTGCCTACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGAGGTCTTGGGACCTGTGGTCCTAATGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCC";
  coords * c1 = new coords(184258374,184258455,184258325,184258475);
  GraphAlignment * ga = new GraphAlignment(g.getSubjectNodes(), q1, M, X, GI, GE, debug);
  Traceback t(ga,c1);


  string q2 = "CCACTTTAGCAAAGTGGAGTGCCTACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGGTGCAAGAGGTCTTGGGACCTGTGGTCCTAATGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCCTTC";
  coords * c2 = new coords(184258374,184258455,184258328,184258478);
  GraphAlignment * ga2 = new GraphAlignment(g.getSubjectNodes(), q2, M, X, GI, GE, debug);
  Traceback t2(ga2,c2);

  string q3 = "ACTTTAGCAAAGTGGAGTGCCTACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCCTTCACCATCTAAGGAAAGGTGTCCCCCCATTCCCA";
  coords * c3 = new coords(184258374,184258455,184258330,184258510);
  GraphAlignment * ga3 = new GraphAlignment(g.getSubjectNodes(), q3, M, X, GI, GE, debug);
  Traceback t3(ga3,c3);

  string q4 = "ACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCCTTCACCATCTAAGGAAAGGTGTCCCCCCATTCCCAATGGTGCGTGGTACACAGAT";
  coords * c4 = new coords(184258374,184258455,184258352,184258530);
  GraphAlignment * ga4 = new GraphAlignment(g.getSubjectNodes(), q4, M, X, GI, GE, debug);
  Traceback t4(ga4,c4);

  string q5 = "CTTTAGCAAAGTGGAGTGCCTACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGAGGGCTTGGGACCTGTGGTCCTAATGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCCTTCACC";
  coords * c5 = new coords(184258374,184258455,184258331,184258481);
  GraphAlignment * ga5 = new GraphAlignment(g.getSubjectNodes(), q5, M, X, GI, GE, debug);
  Traceback t5(ga5,c5);

  string q6 = "ACTTGGAGCAGCAGAGCACACTGAGTACAAGTCCCCTTGCAGCAGAGTTGCAAGATAAGGCCACGGGGCCTGAGGCACCCCTAGACCCTGAGCCTTCACCATCTAAGGAAAGGTGTCCCCCCATTCCCAATGGTGCGTGGTACACAGAT";
  coords * c6 = new coords(184258374,184258455,184258352,184258530);
  GraphAlignment * ga6 = new GraphAlignment(g.getSubjectNodes(), q6, M, X, GI, GE, debug);
  Traceback t6(ga6,c6);

  vector<Traceback> tbv;
  tbv.push_back(t);
  tbv.push_back(t2);
  tbv.push_back(t3);
  tbv.push_back(t4);
  tbv.push_back(t5);
  tbv.push_back(t6);


  //  cout << "inside gsw before Pileup construction\n";
  Pileup p(tbv,subject);

  p.printPileup();
  

  /*
  subject = "AAAAAAAAAACCCCCCCCCCTTTTTTTTTT";
  Variant v = {subject, make_pair("ACCCCCCCCCC","A"), 9}; 
  //Variant v = {subject, make_pair("CCCCCC","C"), 14};
  Graph g(v);
 
  string q1 = "AAAAAAAAAACCCCCCCCCCTTTTTTTTTT";
  coords * c1 = new coords(10,40,10,40);
  GraphAlignment * ga = new GraphAlignment(g.getSubjectNodes(), q1, M, X, GI, GE, debug);
  Traceback t1(ga,c1);    

  string q2 = "AAAAAAAAAATTTTTTTTTTT";
  coords * c2 = new coords(10,40,10,40);
  GraphAlignment * ga2 = new GraphAlignment(g.getSubjectNodes(), q2, M, X, GI, GE, debug);
  Traceback t2(ga2,c2);

  vector<Traceback> tbv;
  
  tbv.push_back(t1);
  tbv.push_back(t2);

  Pileup p(tbv, t2.getQuery());
  p.printPileup(); 
  */
  

  /*  Vector<Node *> matchedNodes = ga->getMatchedNodes();
  cout << "Graph node alignments:" << endl;
  for (vector<Node *>::const_iterator iter = matchedNodes.begin(); iter != matchedNodes.end(); iter++) {
    Node * node = * iter;
    string cigar = ga->getNodeCigar(node);
    int offset = ga->getNodeOffset(node);
    //ga->printMatrix(node, cout);
    cout << "  Node=" << node->getId() << " CIGAR=" << cigar << " offset=" << offset << endl;
    } */
}
