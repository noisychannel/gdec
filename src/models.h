#include <map>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std;
using namespace boost::algorithm;


struct phrase {
  string english;
  double logprob;
};

map <string, vector<phrase>> TM (string filename, unsigned k) {
  cerr << "Reading translation model from " << filename << " ..." << endl;
  map <string, vector<phrase>> tm;
  // Read the translation table into memory
  ifstream tm_file(filename);
  string line, f, e;
  double logprob;
  boost::regex tok_re("\\s\\|\\|\\|\\s");
  while (getline(tm_file, line)) {
    trim(line);
    boost::sregex_token_iterator i(line.begin(), line.end(), tok_re, -1);
    boost::sregex_token_iterator j;
    vector<string> tm_options;
    while (i != j) {
      tm_options.push_back(*i++);
    }
    assert (tm_options.size() == 3);
    phrase newPhrase;
    f = tm_options[0];
    newPhrase.english = tm_options[1];
    newPhrase.logprob = stod(tm_options[2]);
    map <string, vector<phrase>>::iterator it = tm.find(f);
    if (it == tm.end()) {
      vector<phrase> newOpt;
      newOpt.push_back(newPhrase);
      tm.insert(pair<string, vector<phrase>>(f, newOpt));
    }
    else {
      it->second.push_back(newPhrase);
    }
  }

  // Prune and keep top k entries
  for (map <string, vector<phrase>>::iterator it=tm.begin(); it!=tm.end(); ++it) {
    vector<phrase> v = it->second;
    sort(begin(v), end(v),
      [](phrase const &t1, phrase const &t2) {
        //TODO: Should probably be -ve logprob
        return t1.logprob < t2.logprob;
      }
    );
    if (k > v.size()) {
      v.erase(v.begin() + k, v.end());
      assert (v.size() == k);
    }
  }
  return tm;
}

// LM
class LM {
  struct ngram_stats {
    const double logprob;
    const double backoff;
  };

  map < const vector<string>, ngram_stats > table;

  public:
    LM(string lm_filename);
    vector<string> begin();
    vector<string> end(vector<string> state);
    pair<vector<string>, double> score(vector<string> state, string word);
};

LM::LM(string lm_filename) {
  cerr << "Reading language model from " << lm_filename << " ..." << endl;
  ifstream lm_file(lm_filename);
  string line;
  while (getline(lm_file, line)) {
    vector<string> entry;
    boost::split(entry, line, boost::is_any_of("\t"));
    if (entry.size() > 1 and entry[0] != "ngram") {
      double logprob = stod(entry[0]);
      vector<string> ngram;
      boost::split(ngram, entry[1], boost::is_any_of(" "));
      double backoff = 0.0;
      if (entry.size() == 3) {
        backoff = stod(entry[2]);
      }
      ngram_stats new_ns = {logprob, backoff};
      table.insert(pair<const vector<string>, ngram_stats>(ngram, new_ns));
    }
  }
}

vector<string> LM::begin() {
  vector<string> newState;
  newState.push_back("<s>");
  return newState;
}

pair<vector<string>, double> LM::score(vector<string> state, string word) {

}

vector<string> LM::end(vector<string> state) {

}
