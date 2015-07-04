#include <iostream>
#include <stdlib.h>
#include <boost/program_options.hpp>

#include "models.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("input,i", po::value<string>()->default_value("data/input"), "File containing sentences to translate")
      ("tm,t", po::value<string>()->default_value("data/tm"), "File containing translation model")
      ("lm,l", po::value<string>()->default_value("data/lm"), "File containing ARPA-format language model")
      ("num_sentences,n", po::value<int>()->default_value(INT_MAX), "Number of sentences to decode")
      ("trans_per_phrase,k", po::value<int>()->default_value(1), "Limit on number of translations to consider per phrase")
      ("stack_size,s", po::value<int>()->default_value(1), "Maximum stack size")
      ("verbose,v", po::bool_switch()->default_value(false), "Verbose mode")
      ("help", "produce help message")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  string tm_file = vm["tm"].as<string>();
  unsigned k = vm["trans_per_phrase"].as<int>();
  map <string, vector<phrase> > tm = TM(tm_file, k);

  string lm_file = vm["lm"].as<string>();
  LM lm(lm_file);

  struct Hypothesis {
    const double logprob;
    const string lm_state;
    Hypothesis * const predecessor;
    const string phrase;
  };

}

//int main(int argc, char** argv) {
  //if (argc < 2) {
    //cerr << "Usage: " << argv[0] << " kbest.txt" << endl;
    //cerr << endl;
    //cerr << "Where kbest.txt contains lines of them form" << endl;
    //cerr << "sentence_id ||| hypothesis ||| features ||| ... ||| metric score" << endl;
    //cerr << "The first three fields must be the sentence id, hypothesis, and features." << endl;
    //cerr << "The last field must be the metric score of each hypothesis." << endl;
    //cerr << "Any fields in between are ignored." << endl;
    //cerr << endl;
    //cerr << "Here's an example of a valid input line:" << endl;
    //cerr << "0 ||| <s> ovatko ne syyt tai ? </s> ||| MaxLexEgivenF=1.26902 Glue=2 LanguageModel=-14.2355 SampleCountF=9.91427 ||| -1.32408 ||| 21.3" << endl;
    //exit(1);
  //}
//}
