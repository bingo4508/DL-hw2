/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 03.07.04                                                    */
/*                                                                     */
/*   Copyright (c) 2004  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <string.h>
#include "svm_struct/svm_struct_common.h"
#include "svm_struct_api.h"
#include <omp.h>

#include <vector>
#include <string>

using namespace std;


#define SM_PSI_SIZE 5616
#define SM_NUM_FEATURES 69
#define SM_NUM_PHONMES 48

/************ Utility ***********************************/
double dot(double *x, double *y, int size){
    double s=0;
    int i;
    for(i=0;i<size;i++)
        s += x[i]*y[i];
    return s;
}

void ** new_2d_array(int rows, int cols, int size){
    void **a = (void **)calloc(rows, sizeof(void*));
    int i;
    for(i = 0; i < rows; i++)
        a[i] = calloc(cols, size);
    return a;
}

void * new_1d_array(int len, int size){
    void *a = calloc(len, size);
    return a;
}

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

int is_same(int **track, LABEL y, int i){
    // i : index of track[t]
    for (int t=y.n-1; t>0; --t){
	if(i != y.phone[t])
	    return 0;
        i = track[t][i];
    }
    return 1;
}
/******************************************************/


void        svm_struct_learn_api_init(int argc, char* argv[])
{
  /* Called in learning part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_learn_api_exit()
{
  /* Called in learning part at the very end to allow any clean-up
     that might be necessary. */
}

void        svm_struct_classify_api_init(int argc, char* argv[])
{
  /* Called in prediction part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_classify_api_exit()
{
  /* Called in prediction part at the very end to allow any clean-up
     that might be necessary. */
}

SAMPLE      read_struct_examples(char* fname, STRUCT_LEARN_PARM *sparm)
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
              for(int i=0;i<UtteranceN;i++){TempUtterance2[i]=TempUtterance[i];	}
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

void        init_struct_model(SAMPLE sample, STRUCTMODEL *sm, 
			      STRUCT_LEARN_PARM *sparm, LEARN_PARM *lparm, 
			      KERNEL_PARM *kparm)
{
  /* Initialize structmodel sm. The weight vector w does not need to be
     initialized, but you need to provide the maximum size of the
     feature space in sizePsi. This is the maximum number of different
     weights that can be learned. Later, the weight vector w will
     contain the learned weights for the model. */

  sm->sizePsi=SM_PSI_SIZE; /* replace by appropriate number of features */
  sm->num_features = SM_NUM_FEATURES;
  sm->num_phones = SM_NUM_PHONMES;
}

CONSTSET    init_struct_constraints(SAMPLE sample, STRUCTMODEL *sm, 
				    STRUCT_LEARN_PARM *sparm)
{
  /* Initializes the optimization problem. Typically, you do not need
     to change this function, since you want to start with an empty
     set of constraints. However, if for example you have constraints
     that certain weights need to be positive, you might put that in
     here. The constraints are represented as lhs[i]*w >= rhs[i]. lhs
     is an array of feature vectors, rhs is an array of doubles. m is
     the number of constraints. The function returns the initial
     set of constraints. */
  CONSTSET c;
  long     sizePsi=sm->sizePsi;
  long     i;
  WORD     words[2];

  if(1) { /* normal case: start with empty set of constraints */
    c.lhs=NULL;
    c.rhs=NULL;
    c.m=0;
  }
  else { /* add constraints so that all learned weights are
            positive. WARNING: Currently, they are positive only up to
            precision epsilon set by -e. */
    c.lhs=(DOC**)my_malloc(sizeof(DOC *)*sizePsi);
    c.rhs=(double*)my_malloc(sizeof(double)*sizePsi);
    for(i=0; i<sizePsi; i++) {
      words[0].wnum=i+1;
      words[0].weight=1.0;
      words[1].wnum=0;
      /* the following slackid is a hack. we will run into problems,
         if we have move than 1000000 slack sets (ie examples) */
      c.lhs[i]=create_example(i,0,1000000+i,1,create_svector(words,"",1.0));
      c.rhs[i]=0.0;
    }
  }
  return(c);
}

LABEL       classify_struct_example(PATTERN x, STRUCTMODEL *sm,
                                    STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label yhat for pattern x that scores the highest
     according to the linear evaluation function in sm, especially the
     weights sm.w. The returned label is taken as the prediction of sm
     for the pattern x. The weights correspond to the features defined
     by psi() and range from index 1 to index sm->sizePsi. If the
     function cannot find a label, it shall return an empty label as
     recognized by the function empty_label(y). */

    LABEL y;
    y.phone = (int *)new_1d_array(x.n, sizeof(int));
    y.n = x.n;
    y.id = (char *)new_1d_array(30, sizeof(char));
    strcpy(y.id, x.id);

    int num_state = sm->num_phones;
    int num_obsrv = x.n;
    int num_feature = sm->num_features;
    int tran_start = num_feature*num_state;
    int t,j,i;

    double **delta = (double **)new_2d_array(num_obsrv, num_state, sizeof(double));
    int **track = (int **)new_2d_array(num_obsrv, num_state, sizeof(int));

    /* Viterbi */
    // Forwarding
    for (t=0; t<num_obsrv; ++t)
        for (j=0; j<num_state; ++j){
            if (t == 0){
                //log(P{a|x1}) = dot(wa,x1)
                delta[t][j] = dot(&sm->w[j*num_feature+1], x.utterance[t], num_feature);
            }else{
                double p = -1e9;
                for (i=0; i<num_state; ++i){
                    double w = delta[t-1][i] + sm->w[tran_start+num_state*i+j+1];
                    if (w > p){
                        p = w;
                        track[t][j] = i;
                    }
                }
                delta[t][j] = p + dot(&sm->w[j*num_feature+1], x.utterance[t], num_feature);
#ifdef DEBUG
	printf("t:%d\tj:%d\tp:%f\tdot:%f\n",t,j,p,dot(&sm->w[j*num_feature], x.utterance[t], num_feature));
#endif
            }
        }

    // Back-tracking
    double p = -1e9;
    for (j=0; j<num_state; ++j)
        if (delta[num_obsrv-1][j] > p){
            p = delta[num_obsrv-1][j];
            y.phone[num_obsrv-1] = j;
	}

    for (t=num_obsrv-1; t>0; --t){
        y.phone[t-1] = track[t][y.phone[t]];
    }

    // Free memory
    for(i=0;i<num_obsrv;++i){
	free(delta[i]);
	free(track[i]);
    }
    free(delta);
    free(track);

    return y;
}


LABEL       find_most_violated_constraint_slackrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the slack rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)*(1-psi(x,y)+psi(x,ybar)) 

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
#ifdef DEBUG
  printf("find-most_violated_constraint_slackrescaling\n");
#endif
  LABEL ybar;
  ybar.n = 0;

  /* insert your code for computing the label ybar here */

  return(ybar);
}

LABEL       find_most_violated_constraint_marginrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the margin rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)+psi(x,ybar)

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
#ifdef DEBUG
  printf("find-most_violated_constraint_marginrescaling\n");
#endif
 
    LABEL ybar;
  /* insert your code for computing the label ybar here */
    ybar.phone = (int *)new_1d_array(x.n, sizeof(int));
    ybar.n = x.n;

    int num_state = sm->num_phones;
    int num_obsrv = x.n;
    int num_feature = sm->num_features;
    int tran_start = num_feature*num_state;
    int t;

    
    double **delta = (double **)new_2d_array(num_obsrv, num_state, sizeof(double));
    int **track = (int **)new_2d_array(num_obsrv, num_state, sizeof(int));

    /* Viterbi */
    // Forwarding
    for (t=0; t<num_obsrv; ++t)
        for (int j=0; j<num_state; ++j){
            if (t == 0){
                //log(P{a|x1}) = dot(wa,x1)
                delta[t][j] = dot(&sm->w[j*num_feature+1], x.utterance[t], num_feature);
            }else{
                double p = -1e9;
                for (int i=0; i<num_state; ++i){
                    double w = delta[t-1][i] + sm->w[tran_start+num_state*i+j+1];
		    if(w > p){
			if(sparm->loss_function==0){
                    	    if(t == num_obsrv-1 && j == y.phone[y.n-1] && i == y.phone[y.n-2] && is_same(track, y, j))
			    {
                            	if(w-1 > p)
				{
				    p = w-1;
			    	    track[t][j] = i;
				}
			    }else{
                                p = w;
                                track[t][j] = i;
                            }
                	}else{
			    p = w;
			    track[t][j] = i;
			}
		    }
                }
                delta[t][j] = p + dot(&sm->w[j*num_feature+1], x.utterance[t], num_feature);
            }
  	    /* handle non 0/1 loss */
	    if(sparm->loss_function > 0){
                if(y.phone[t] != j)
                    delta[t][j]+=(float)1.0/num_obsrv;
	    }
    }
    
    // Back-tracking
    double p = -1e9;
    for (int j=0; j<num_state; ++j)
        if (delta[num_obsrv-1][j] > p){
            p = delta[num_obsrv-1][j];
            ybar.phone[num_obsrv-1] = j;
        }    

    for (t=num_obsrv-1; t>0; --t)
        ybar.phone[t-1] = track[t][ybar.phone[t]];

    // Free memory
    for(int i=0;i<num_obsrv;++i){
        free(delta[i]);
        free(track[i]);
    }
    free(delta);
    free(track);

    return(ybar);
}

int         empty_label(LABEL y)
{
  /* Returns true, if y is an empty label. An empty label might be
     returned by find_most_violated_constraint_???(x, y, sm) if there
     is no incorrect label that can be found for x, or if it is unable
     to label x at all */
#ifdef DEBUG
  printf("empty_label\n");
#endif
 
  return(0);
}


SVECTOR     *psi(PATTERN x, LABEL y, STRUCTMODEL *sm,
		 STRUCT_LEARN_PARM *sparm)
{
    
  /* Returns a feature vector describing the match between pattern x
     and label y. The feature vector is returned as a list of
     SVECTOR's. Each SVECTOR is in a sparse representation of pairs
     <featurenumber:featurevalue>, where the last pair has
     featurenumber 0 as a terminator. Featurenumbers start with 1 and
     end with sizePsi. Featuresnumbers that are not specified default
     to value 0. As mentioned before, psi() actually returns a list of
     SVECTOR's. Each SVECTOR has a field 'factor' and 'next'. 'next'
     specifies the next element in the list, terminated by a NULL
     pointer. The list can be though of as a linear combination of
     vectors, where each vector is weighted by its 'factor'. This
     linear combination of feature vectors is multiplied with the
     learned (kernelized) weight vector to score label y for pattern
     x. Without kernels, there will be one weight in sm.w for each
     feature. Note that psi has to match
     find_most_violated_constraint_???(x, y, sm) and vice versa. In
     particular, find_most_violated_constraint_???(x, y, sm) finds
     that ybar!=y that maximizes psi(x,ybar,sm)*sm.w (where * is the
     inner vector product) and the appropriate function of the
     loss + margin/slack rescaling method. See that paper for details. */
     
  //SVECTOR *fvec=(SVECTOR*) malloc(sizeof(SVECTOR));
  //cout << "hey. \n"; 
  WORD *TempWord = (WORD*) malloc(sizeof(WORD)*((sm->num_features)*(sm->num_phones)+(sm->num_phones)*(sm->num_phones)+1));
  int lastLabel=-1;
  int currentLabel=-1;
   
  
  for(int i=1;i<(sm->num_features)*(sm->num_phones)+(sm->num_phones)*(sm->num_phones)+1;i++)
  {
          TempWord[i-1].wnum=i;
          TempWord[i-1].weight=0;
  }
  TempWord[(sm->num_features)*(sm->num_phones)+(sm->num_phones)*(sm->num_phones)].wnum=0;
  
  for(int i=0;i<x.n;i++)
  {
   for(int j=0;j<(sm->num_features);j++)
   {
           //cout <<x.utterance[i][0]<<endl; 

           TempWord[j+(sm->num_features)*y.phone[i]].weight+=x.utterance[i][j];

           
   }  
           currentLabel=y.phone[i];
           if(lastLabel>=0)
           {
           TempWord[(sm->num_features)*(sm->num_phones)+(sm->num_phones)*lastLabel+currentLabel].weight++; 
           }
           lastLabel=currentLabel;     
  }
  char* a="";
  SVECTOR *fvec=create_svector(TempWord,a,1.0); 
  free(TempWord);
  /* insert code for computing the feature vector for x and y here */

  return(fvec);
}

double      loss(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm)
{
  int i;
  int s=0;
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  if(sparm->loss_function == 0) { /* type 0 loss: 0/1 loss */
                                  /* return 0, if y==ybar. return 1 else */
        for(i=0;i<y.n;i++)
            if(y.phone[i] != ybar.phone[i])
                return 1;
        return 0;
  }
  else {
    /* Put your code for different loss functions here. But then
       find_most_violated_constraint_???(x, y, sm) has to return the
       highest scoring label with the largest loss. */
        for(i=0;i<y.n;i++)
            if(y.phone[i] != ybar.phone[i])
                s++;
        return s;
  }
}

int         finalize_iteration(double ceps, int cached_constraint,
			       SAMPLE sample, STRUCTMODEL *sm,
			       CONSTSET cset, double *alpha, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* This function is called just before the end of each cutting plane iteration. ceps is the amount by which the most violated constraint found in the current iteration was violated. cached_constraint is true if the added constraint was constructed from the cache. If the return value is FALSE, then the algorithm is allowed to terminate. If it is TRUE, the algorithm will keep iterating even if the desired precision sparm->epsilon is already reached. */
#ifdef DEBUG
  printf("finalize_iteration\n");
#endif
 
  return(0);
}

void        print_struct_learning_stats(SAMPLE sample, STRUCTMODEL *sm,
					CONSTSET cset, double *alpha, 
					STRUCT_LEARN_PARM *sparm)
{
  /* This function is called after training and allows final touches to
     the model sm. But primarly it allows computing and printing any
     kind of statistic (e.g. training error) you might want. */
}

void        print_struct_testing_stats(SAMPLE sample, STRUCTMODEL *sm,
				       STRUCT_LEARN_PARM *sparm, 
				       STRUCT_TEST_STATS *teststats)
{
  /* This function is called after making all test predictions in
     svm_struct_classify and allows computing and printing any kind of
     evaluation (e.g. precision/recall) you might want. You can use
     the function eval_prediction to accumulate the necessary
     statistics for each prediction. */
}

void        eval_prediction(long exnum, EXAMPLE ex, LABEL ypred, 
			    STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, 
			    STRUCT_TEST_STATS *teststats)
{
  /* This function allows you to accumlate statistic for how well the
     predicition matches the labeled example. It is called from
     svm_struct_classify. See also the function
     print_struct_testing_stats. */
  if(exnum == 0) { /* this is the first time the function is
		      called. So initialize the teststats */
  }
}

void        write_struct_model(char *file, STRUCTMODEL *sm, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* Writes structural model sm to file file. */
  FILE *fp = fopen(file, "w");
  fprintf(fp, "%ld\n", sm->sizePsi);
  /*fprintf(fp, "%lf\n", sm->walpha);
  fprintf(fp, "%d\n", sm->num_features);
  fprintf(fp, "%d\n", sm->num_phones);

  fprintf(fp, "%ld\n", sm->svm_model->sv_num);
  fprintf(fp, "%ld\n", sm->svm_model->at_upper_bound);
  fprintf(fp, "%lf\n", sm->svm_model->b);*/
  int i;
  /*
  for (i = 0; i < sm->svm_model->sv_num; i++)
	fprintf(fp, "%lf ", sm->svm_model->alpha[i]);
  fprintf(fp, "\n");
  */
  fprintf(fp, "%ld\n", sm->svm_model->kernel_parm.kernel_type);
  for (i = 0; i < sm->sizePsi; i++)
  	fprintf(fp, "%e ", sm->w[i+1]);
  fclose(fp);
}

STRUCTMODEL read_struct_model(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads structural model sm from file file. This function is used
     only in the prediction module, not in the learning module. */
     STRUCTMODEL sm;
	 sm.sizePsi=SM_PSI_SIZE; /* replace by appropriate number of features */
	 sm.num_features = SM_NUM_FEATURES;
	 sm.num_phones = SM_NUM_PHONMES;
	 sm.svm_model = (MODEL*)my_malloc(sizeof(MODEL));
     FILE *fp = fopen(file, "r");
     fscanf(fp, "%ld", &sm.sizePsi);
     fscanf(fp, "%ld", &sm.svm_model->kernel_parm.kernel_type);
     sm.w = (double*)my_malloc(sizeof(double)*(sm.sizePsi+1));
	 int i;
     for (i = 0; i < sm.sizePsi; i++)
     	fscanf(fp, "%lf", &sm.w[i+1]);
     fclose(fp);
     return sm;
}

void        write_label(FILE *fp, LABEL y)
{
  /* Writes label y to file handle fp. */
  int map_48_39_idx[] = { 0, 1, 2, 0, 4, 2, 6, 1, 8, 37, 10, 11, 12, 13, 27, 29, 37, 17, 18, 19, 20, 21, 22, 22, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 37, 44, 45, 46, 36 };
  fprintf(fp, "%s %d ", y.id, y.n);
  int i;
  for (i = 0; i < y.n; i++)
  	fprintf(fp, "%d ", map_48_39_idx[y.phone[i]]);
  fprintf(fp, "\n");
} 

void outputResult(FILE *beforeTrim, char *afterTrim)
{
	FILE *outFp = fopen(afterTrim, "w");
	fprintf(outFp, "id,phone_sequence\n");
	rewind(beforeTrim);
	
	char id[40];
	int phonmeIdx;
	int frameSize;
	int isSilHead;
	int prePhmIdx;
	int i;
	while(fscanf(beforeTrim, "%s ", id) != EOF)
	{
		fprintf(outFp, "%s,", id);
		fscanf(beforeTrim, "%d", &frameSize);
		isSilHead = 1;
		prePhmIdx = -1;
		for (i = 0; i < frameSize; i++)
		{
			fscanf(beforeTrim, "%d", &phonmeIdx);
			if (isSilHead && phonmeIdx != 37)	//first not-sil phonme
				isSilHead = 0;
			if (!isSilHead && prePhmIdx != phonmeIdx)
			{
				if (phonmeIdx != 37)
				{
					if (prePhmIdx == 37)
						fprintf(outFp, "L");
					if (phonmeIdx < 26)
						fprintf(outFp, "%c", (char)(phonmeIdx+'a'));
					else
						fprintf(outFp, "%c", (char)(phonmeIdx-26+'A'));
				}
				prePhmIdx = phonmeIdx;
			}
		}
		fprintf(outFp, "\n");
	}
	fclose(outFp);
}

void        free_pattern(PATTERN x) {
  /* Frees the memory of x. */
    int i;
    for(i=0;i<x.n;++i)
        free(x.utterance[i]);
    free(x.id);
    free(x.utterance);
}

void        free_label(LABEL y) {
  /* Frees the memory of y. */
    //free(y.id);
    free(y.phone);
}

void        free_struct_model(STRUCTMODEL sm) 
{
  /* Frees the memory of model. */
  /* if(sm.w) free(sm.w); */ /* this is free'd in free_model */
  if(sm.svm_model) free_model(sm.svm_model,1);
  /* add free calls for user defined data here */
}

void        free_struct_sample(SAMPLE s)
{
  /* Frees the memory of sample s. */
  int i;
  for(i=0;i<s.n;i++) { 
    free_pattern(s.examples[i].x);
    free_label(s.examples[i].y);
  }
  free(s.examples);
}

void        print_struct_help()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_learn. */
  printf("         --* string  -> custom parameters that can be adapted for struct\n");
  printf("                        learning. The * can be replaced by any character\n");
  printf("                        and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      case 'a': i++; /* strcpy(learn_parm->alphafile,argv[i]); */ break;
      case 'e': i++; /* sparm->epsilon=atof(sparm->custom_argv[i]); */ break;
      case 'k': i++; /* sparm->newconstretrain=atol(sparm->custom_argv[i]); */ break;
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

void        print_struct_help_classify()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_classify. */
  printf("         --* string -> custom parameters that can be adapted for struct\n");
  printf("                       learning. The * can be replaced by any character\n");
  printf("                       and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters_classify(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- for the
     classification module */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      /* case 'x': i++; strcpy(xvalue,sparm->custom_argv[i]); break; */
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

