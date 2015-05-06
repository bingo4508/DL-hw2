#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

#define SM_PSI_SIZE 5616
#define SM_NUM_FEATURES 69

typedef struct pattern {
  int n; //num of frames
  double **utterance;
  char *id;
} PATTERN;

typedef struct label {
  /* this defines the y-part (the label) of a training example,
     e.g. the parse tree of the corresponding sentence. */
  char *id;
  int n;
  int *phone;
} LABEL;

typedef struct example {  /* an example is a pair of pattern and label */
  PATTERN x;
  LABEL y;
} EXAMPLE;

typedef struct sample { /* a sample is a set of examples */
  int     n;            /* n is the total number of examples */
  EXAMPLE *examples;
} SAMPLE;


vector<string> split(string str,string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    vector<string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

SAMPLE      read_struct_examples(char* fname)
{
  /* Reads struct examples and returns them in sample. The number of
     examples must be written into sample.n */
  SAMPLE   sample;  /* sample */
  EXAMPLE  *examples;
  long     n;       /* number of examples */

  n=0; /* replace by appropriate number of examples */
  

  
  EXAMPLE temp[5000];
  
  /* fill in your code here */
  fstream fin;
  vector<string> CurrentNameT;
  string CurrentName;
  string LastName="Initial";
  int UtteranceN=0;
  int TempLabel;
  double *TempUtterance[1000];
  int TempLabelS[1000];
  ifstream input(fname, ifstream::in);
  ifstream input2(fname, ifstream::in);
  string line2;
  getline(input2, line2);
  
        for(string line; getline(input, line);){
                
          string line2;
          getline(input2, line2);
          //cout << line<< "\n"; 
                  vector<string> x =split(line, " ");
                  int feature_size = SM_NUM_FEATURES;//x.size()-2;
                  double *TempVector = (double*)malloc(sizeof(double)*(feature_size));   /////////////////////////////my_malloc

                  
                  CurrentNameT=split(x[0], "_");
          CurrentName=CurrentNameT[0]+"_"+CurrentNameT[1];        
                  for (int i = 1; i<feature_size+1; i++){ TempVector[i - 1] = atof(x[i].c_str()); }
                  TempLabel=atoi(x[x.size()-1].c_str());
                  
              if((CurrentName==LastName||LastName=="Initial")&&!input2.eof())
              {     
                TempUtterance[UtteranceN]=TempVector;
                TempLabelS[UtteranceN]=TempLabel;
                UtteranceN++;                                    
          }
          else
          {   
              //double *TempUtterance2[UtteranceN];
              double **TempUtterance2 = (double**) malloc(sizeof(double*)*UtteranceN); /////////////////////////////my
              //int TempLabelS2[UtteranceN];
              int *TempLabelS2 = (int*) malloc(sizeof(int)*UtteranceN);   /////////////////////////////my
              for(int i=0;i<UtteranceN;i++){TempUtterance2[i]=TempUtterance[i]; }
              for(int i=0;i<UtteranceN;i++){TempLabelS2[i]=TempLabelS[i];}
              temp[n].x.utterance=TempUtterance2;
              temp[n].x.n=UtteranceN;
              temp[n].x.id=(char*)malloc(sizeof(char)*30); 
              strcpy(temp[n].x.id,LastName.c_str());
              temp[n].y.phone=TempLabelS2;
              temp[n].y.n=UtteranceN;
              temp[n].y.id=(char*)malloc(sizeof(char)*30); 
              strcpy(temp[n].y.id,LastName.c_str());
              n++;
              UtteranceN=0;
              TempUtterance[UtteranceN]=TempVector;
              TempLabelS[UtteranceN]=TempLabel;
              UtteranceN++;                  
          }
          LastName=CurrentName;
    }
  examples=(EXAMPLE *)malloc(sizeof(EXAMPLE)*n);//my
  for(int i=0;i<n;i++){examples[i]=temp[i]; }
  sample.n=n;             
  sample.examples=examples;
  return(sample);
}

void split_valid(char *input, float valid_ratio)
{
	string str(input);
	puts("Loading data...");
	SAMPLE s = read_struct_examples(input);	
	ofstream fv((str+".val").c_str());
	ofstream ft((str+".train").c_str());

	printf("Spliting... Top %f%% is validation\n",valid_ratio*100);
	int i;
	EXAMPLE ex;
	for(i=0;i<valid_ratio*s.n;i++){
	    ex = s.examples[i];
	    for(int j=0;j<ex.x.n;j++){
	        fv << ex.x.id << "_" << j+1 << " ";
		for(int k=0;k<SM_NUM_FEATURES;k++)
	            fv << std::fixed << std::setprecision(6) <<ex.x.utterance[j][k] << " ";
		fv << endl;
	    }
	}
	for(;i<s.n;i++){
	    ex = s.examples[i];
	    for(int j=0;j<ex.x.n;j++){
	        ft << ex.x.id << "_" << j+1 << " ";
		for(int k=0;k<SM_NUM_FEATURES;k++)
	            ft <<std::fixed << std::setprecision(6)<< ex.x.utterance[j][k] << " ";
		ft << endl;
	    }
	}	
}

int main(int argc, char* argv[])
{
	char *input = argv[1];
	float valid_ratio = atof(argv[2]);
	
	split_valid(input, valid_ratio);
	return 0;
}







