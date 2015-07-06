#include <iostream>
#include <fstream>
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
      ("num_sentences,n", po::value<unsigned>()->default_value(INT_MAX), "Number of sentences to decode")
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
  map <vector<string>, vector<phrase> > tm = TM(tm_file, k);

  string lm_file = vm["lm"].as<string>();
  LM lm(lm_file);

  const string input_file = vm["input"].as<string>();
  cerr << "Decoding " << input_file << " ..." << endl;

  vector<vector<string> > french;
  ifstream f_file(input_file.c_str());
  string f_line;
  while (getline(f_file, f_line)) {
    // Respect number of lines to decode
    if (french.size() > vm["num_sentences"].as<unsigned>()) {
      break;
    }
    trim(f_line);
    vector<string> f_tokens;
    boost::split(f_tokens, f_line, boost::is_any_of(" "));
    french.push_back(f_tokens);
  }

  struct Hypothesis {
    double logprob;
    vector<string> lm_state;
    Hypothesis * predecessor;
    string phrase;
  };

  for (vector<vector<string> >::iterator f_it = french.begin(); f_it != french.end(); ++f_it) {
    vector<string> f = *f_it;
    Hypothesis initial_hypothesis = {0.0, lm.begin(), NULL, ""};
    vector<vector<pair<vector<string>, Hypothesis> > > stacks;
    for (unsigned i = 0; i < f.size() + 1; i++) {
      vector<pair<vector<string>, Hypothesis>> emptyStack;
      stacks.push_back(emptyStack);
    }
    stacks[0].push_back(make_pair(lm.begin(), initial_hypothesis));

    for (unsigned i = 0; i < stacks.size() - 1; ++i) {
      vector<pair<vector<string>, Hypothesis>> stack = stacks[i];
      // Sort first
      sort(stack.begin(), stack.end(),
        [](const pair<vector<string>, Hypothesis>& t1, const pair<vector<string>, Hypothesis>& t2) {
          // The -ve ensures descending order, instead of the default asc
          return -1.0 * t1.second.logprob < -1.0 * t2.second.logprob;
        }
      );
      // Prune
      if (stack.size() > k) {
        stack.erase(stack.begin() + k, stack.end());
        assert (stack.size() == k);
      }

      for (vector<pair<vector<string>, Hypothesis>>::iterator hyp_it = stack.begin();
          hyp_it != stack.end(); ++hyp_it) {
        Hypothesis h = (*hyp_it).second;
        for (unsigned j = i+1; j < f.size() + 1; ++j) {
          vector<string> span(&f[i], &f[j]);
          //TODO: Change TM to have vector<string> as key
          map<vector<string>, vector<phrase>>::iterator span_it = tm.find(span);
          if (span_it != tm.end()) {
            vector<phrase> phrases = span_it->second;
            for (vector<phrase>::iterator phrase_it = phrases.begin();
                phrase_it != phrases.end(); ++phrase_it) {
              phrase p = *phrase_it;
              double logprob = h.logprob + p.logprob;
              vector<string> lm_state = h.lm_state;
              //logprob = h.logprob + phrase.logprob
              //lm_state = h.lm_state
              //for word in phrase.english.split():
                //(lm_state, word_logprob) = lm.score(lm_state, word)
                //logprob += word_logprob
              //logprob += lm.end(lm_state) if j == len(f) else 0.0
              //new_hypothesis = hypothesis(logprob, lm_state, h, phrase)
              //if lm_state not in stacks[j] or stacks[j][lm_state].logprob < logprob: # second case is recombination
                //stacks[j][lm_state] = new_hypothesis 
            }
          }
        }
      }
    }
  }
}
