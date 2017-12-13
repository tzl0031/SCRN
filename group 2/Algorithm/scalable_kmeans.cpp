#include <string.h>
#include <set>
#include <stack>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "math.h"
#include "mex.h"

//#define _MEX_DEBUG
// we add an internal structure to map from features to clusters

using namespace std;

typedef map<int, double> INST;
typedef vector<INST> MINST;
typedef INST::iterator IINST;

int MAX_ITER = 100;
double TOL = 0.001;

void printINST(INST inst){
    IINST it;
    double len = 0;
    for (it = inst.begin(); it!=inst.end(); it++){
        mexPrintf("%d:%g ", it->first, it->second);
        len +=it->second * it->second;
    }
    len = sqrt(len);
    mexPrintf("len=%g", len);
    mexPrintf("\n");
}


class SparseMatrix {
    const mxArray * array_ptr;
    mwSize  n;
    mwSize  m;
    double * pr;
    mwSize * ir, *jc;

    mwSize * feature2inst;  // of size  nz
    mwSize * featureIndex;  // of size (m+1)


    public:
        SparseMatrix(const mxArray *  sparse_array_ptr) {
            array_ptr = sparse_array_ptr;
            pr = mxGetPr(array_ptr);
            ir = mxGetIr(array_ptr);
            jc = mxGetJc(array_ptr);
            m = mxGetM(array_ptr);
            n = mxGetN(array_ptr);

	    feature2inst = NULL;
            featureIndex = NULL;
	    
	    getFeature2InstMap();      	    
        }
       
         ~SparseMatrix() {
	   if (feature2inst !=NULL){
                delete [] feature2inst;
            }
            if (featureIndex !=NULL){
                delete [] featureIndex;
            }
	 }
  
        int getM(){
            return m;
        }
        
        int getN(){
            return n;
        }

            int getNZ(){
            return jc[n];
        }
    

 // given data, generate feature-> inst map, this would be used to build a inverse index table for each cluster
        void getFeature2InstMap(){
            
            // transpose instrance matrix to obtain feature->inst map;
            
            
            mxArray * prhs[1], * plhs[1];
            prhs[0] = mxDuplicateArray(array_ptr);
            if (mexCallMATLAB(1, plhs, 1, prhs, "transpose")){
                mexPrintf("Error: cannot transpose training instance matrix \n");
                return;
            }
            
            
            mwSize * feaIndex = mxGetJc(plhs[0]);
            mwSize * fea2inst = mxGetIr(plhs[0]);
            
            int nz = getNZ();
            feature2inst =  new mwSize[nz];
            featureIndex = new mwSize[m+1];
            memcpy(feature2inst, fea2inst, sizeof(mwSize)*nz);
            memcpy(featureIndex, feaIndex, sizeof(mwSize)*(m+1));
            
            //             for (int i=0; i<nz; i++){
            //                 mexPrintf("%d\n", feature2inst[i]);
            //             }
            //destroy the transpose to save memory
            mxDestroyArray(plhs[0]);
            
	}
        
      // set the inverse table based on the features in the centroid
        void setInverseTable(INST &   centroid, set<int> & relevantInsts){
            int fIndex, inst;
            for(IINST it = centroid.begin();  it != centroid.end(); it++){
                fIndex = it->first;
                for (mwSize i=featureIndex[fIndex]; i<featureIndex[fIndex+1]; i++){
                    inst = feature2inst[i];
                    if(relevantInsts.find(inst) == relevantInsts.end()){
                        relevantInsts.insert(inst);
                    }
                }
            }
            
        }
      
       
        
        // calculate the centroid between two instances
        double similarity(int index,  INST &  centroid){
            mwIndex   starting_row_index, stopping_row_index, current_row_index;
            starting_row_index = jc[index];
            stopping_row_index = jc[index+1];
            int rIndex;
            double val;
            double sim=0;
            if (starting_row_index == stopping_row_index)
                return 0.0;
            
            for (current_row_index = starting_row_index; current_row_index < stopping_row_index;
            current_row_index++){
                rIndex = ir[current_row_index];
                val = pr[current_row_index];
                if (centroid.find(rIndex)!=centroid.end()){
                    sim += val * centroid[rIndex];
                }
            }
            return sim;
            
        }
        
        // convert data instance to INST
        INST data2Inst(int index){
            mwIndex   starting_row_index, stopping_row_index, current_row_index;
            starting_row_index = jc[index];
            stopping_row_index = jc[index+1];
            int rIndex;
            double val;
            INST inst = INST();
            for(current_row_index = starting_row_index; current_row_index < stopping_row_index;
            current_row_index++){
                rIndex = ir[current_row_index];
                val = pr[current_row_index];
                inst[rIndex] = val;
            }
            return inst;
        }
        
        // update the centroid
        void updateCentroid(int index, INST & centroid){
            mwIndex   starting_row_index, stopping_row_index, current_row_index;
            starting_row_index = jc[index];
            stopping_row_index = jc[index+1];
            int rIndex;
            double val;
            
            for (current_row_index = starting_row_index; current_row_index < stopping_row_index;
            current_row_index++){
                rIndex = ir[current_row_index];
                val = pr[current_row_index];
                if (centroid.find(rIndex)!=centroid.end()){
                    centroid[rIndex] = centroid[rIndex]+val;
                }else{
                    centroid[rIndex] = val;
                }
            }
        }
        
};

int generateRandom(int highest, int lowest=0){
    int random_integer=-1;
    int range=(highest-lowest)+1;
    
    random_integer = lowest+int(range*(rand()/(RAND_MAX + 1.0)));
    //mexPrintf("%d\n", random_integer);
    //cout << rand() << ' '<< range<<' ' << random_integer <<endl;
    return random_integer;
}

INST pickOneInst(SparseMatrix & data){
    int highest = data.getN()-1;
    int lowest = 0;
    int idx = generateRandom(highest, lowest);
    return data.data2Inst(idx);
}

//random pick some data as centroids
MINST initializeCentroids(SparseMatrix & data, int k){
    int highest = data.getN()-1;
    int lowest = 0;
    MINST  centroids = MINST(k);
    for (int i=0; i<centroids.size(); i++){
        int idx = generateRandom(highest, lowest);
        //mexPrintf("%d\n", idx);
        centroids[i] = data.data2Inst(idx);
    }
    return centroids;
}



double assignClusterWithFeatureMapping(SparseMatrix & data, MINST & centroids, int * idx, stack<int> & isolated){
  set<int>::iterator it;
 
  int n = data.getN();
  memset(idx, 0, sizeof(int) * n);
  
  double * maxsim = new double[n]; 
  memset(maxsim, 0, sizeof(double)*n);
  
  double sim;
  int instIndex; 
  
  for (int i=0; i<centroids.size(); i++){
    
    if (i%100==0){
      mexPrintf("Processed %d clusters...\n", i);
    }
    // construct relevant instance set
    set <int> ReInsts = set<int>();
    data.setInverseTable(centroids[i], ReInsts);
    
#ifdef _MEX_DEBUG
    mexPrintf("ReInsts : % d instances\n", ReInsts.size());
#endif
    
    // calculate similarity
    for(it = ReInsts.begin(); it!=ReInsts.end(); it++){
      //mexPrintf("Reach here...\n");
      instIndex = *it;
      sim = data.similarity(instIndex, centroids[i]);
      //mexPrintf("%d: sim=%g\n", *it, sim);
     
      if (sim > maxsim[instIndex]){
	maxsim[instIndex] = sim;
	idx[instIndex] = i;
      }
    }
    ReInsts.clear();
  }
    
  while(!isolated.empty()){
    isolated.pop();
  }
  
  double obj = 0; 
  for (int i=0; i<data.getN(); i++){
    obj += maxsim[i];
    if (maxsim[i] < 1e-9){
      //mexPrintf("Encounter isolated nodes\n");
      isolated.push(i);
    }
  }
  return obj;
}


double assignCluster(SparseMatrix & data, MINST & centroids, int * idx){
    int numCluster = centroids.size();
    double sim = 0;
    double maxSim = -1;
    int maxIdx = -1;
    double obj = 0;
    for (int i=0; i<data.getN(); i++){
        maxSim = -1;
        maxIdx = -1;
        for (int j=0; j<centroids.size(); j++){
            sim = data.similarity(i,centroids[j]);
            //mexPrintf("i=%d, cluster=%d, sim=%f\n", i, j, sim);
            if  (sim >  maxSim){
                maxSim = sim;
                maxIdx = j;
            }
        }
        idx[i] = maxIdx;
        obj += maxSim;
    }
    return obj;
}

/* convert into unit length */
void normalize(MINST & centroids){
    IINST it;
    for (int i=0; i<centroids.size(); i++){
        INST & inst = centroids[i];
        double len = 0;
        for (it = inst.begin(); it!=inst.end(); it++){
            len += it->second * it->second;
        }
        if (len > 0){
            len = 1/sqrt(len);
            for (it = inst.begin(); it!=inst.end(); it++){
                inst[it->first] = it->second * len;
            }
        }
    }
}


// void udpateCentroidsWithFeatureMapping(SparseMatrix & data, MINST & centroids, int * idx){
//   int i;
//   int k = centroids.size();
//   int m = data.getM();
  
//   for (i=0; i<centroids.size(); i++){
//     centroids[i].clear();  // clear the centroids
//   }
  
//   int * flag = new int[k];
//   double * val = new double[k];
      
//   // iterate over features 
//   for (i=0; i<data.getM(); i++){
    
//     memset(val, 0, sizeof(double)*k);
//     memset(flag, 0, sizeof(int) * k);
    
//     // iterate over relevant instances
    
//   }
  
  

// }


// randomly pick one instance, consider those isolated ones first
INST pickOneInst(SparseMatrix & data, stack <int> & isolated){
  int idx;
  
  if (!isolated.empty()){
    idx = isolated.top();
    isolated.pop();
    //#ifdef _MEX_DEBUG
    //mexPrintf("Pick one isolated instance: %d, still %d remains\n", idx, isolated.size());
    //#endif    
  }else{
    int highest = data.getN()-1;
    int lowest = 0;
    idx = generateRandom(highest, lowest);
  }
    return data.data2Inst(idx);
}


// update centroids
void updateCentroids(SparseMatrix & data, MINST & centroids, int * idx, int * numCluster, stack<int> & isolated){
    int i;
    
    memset(numCluster, 0, sizeof(int) * centroids.size());

    for (i=0; i< centroids.size(); i++){
      centroids[i].clear();  // clear the centroids
      //INST(centroids[i]).swap(centroids[i]);
    }
    
    for (i=0; i< data.getN(); i++){
        numCluster[idx[i]]++;
	if (i%1000000==0){mexPrintf("Processed %d instances\n", i);}
        data.updateCentroid(i, centroids[idx[i]]);
    }

    
    //check if there's any empty cluster
    for (i=0; i<centroids.size(); i++){
        // mexPrintf(" i= %d, numcluster = %d\n", i, numCluster[i]);
        if (0 == numCluster[i] ){
	  centroids[i] = pickOneInst(data, isolated);
            //set the corresponding inverse table
        }
    }
        
    //renormalize the centroids into unit length
    normalize(centroids);
}

// the main clustering process
void mainCluster(SparseMatrix &  data, int k, mxArray *plhs[]){
    mexPrintf("data size: %d %d\n", data.getM(), data.getN());
    
    
    // initialization
    MINST centroids = initializeCentroids(data, k);
    normalize(centroids);
    //         for (int i=0; i<centroids.size(); i++){
    //             printINST(centroids[i]);
    //         }

    double obj, obj_p=0, diff;
    int * idx = new int[data.getN()];
    int * clusterSize = new int[k];
    int iter;
    stack<int> isolated = stack<int>(); 

    mexPrintf("Start clustering ...\n");
    for(iter = 0; iter<MAX_ITER; iter++){
      mexPrintf("Assign Clusters ...\n");
      obj = assignClusterWithFeatureMapping(data, centroids, idx, isolated);
      diff = obj-obj_p;
        //if (iter % 5 == 0){
      mexPrintf("iter: %d, obj=%g, diff = %g obj*tol=%g\n", iter, obj, diff, obj*TOL);
      //}
      if (diff < obj*TOL){
	break;
      }
      obj_p = obj;
        
        //         for (int i=0; i<data.getN(); i++){
        //             mexPrintf("%d\n", idx[i]);
        //         }
        //         mexPrintf("obj = %g\n", obj);
        // do assignment
      mexPrintf("Update Centroids ...\n");
      // recalculate the centroid
      updateCentroids(data, centroids, idx, clusterSize, isolated);
        //         for (int i=0; i<centroids.size(); i++){
        //             printINST(centroids[i]);
        //         }
    }
    
    //convert the idx and   numcluster into matlab format
    plhs[0] = mxCreateDoubleMatrix(data.getN(), 1, mxREAL);
    double * p = mxGetPr(plhs[0]);
    for (int i=0; i<data.getN(); i++){
        p[i] = idx[i];
    }
    delete []idx;
    
    
    plhs[1] = mxCreateDoubleMatrix(k, 1, mxREAL);
    p = mxGetPr(plhs[1]);
    for (int i=0; i<centroids.size(); i++){
        p[i] = clusterSize[i];
    }
    delete []clusterSize;
    
    // release unnecessary memory
    for (int i=0;  i< centroids.size(); i++){
        centroids[i].clear();
    }
    
    plhs[2] = mxCreateDoubleMatrix(1, 1, mxREAL);
    p = mxGetPr(plhs[2]);
    *p = iter;
    
    plhs[3] = mxCreateDoubleMatrix(1, 1, mxREAL);
    p = mxGetPr(plhs[3]);
    p[0] = obj;
}


void mexFunction(
int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[])
{
  //srand((unsigned)time(0));
  srand(1);
  
    if (nrhs < 2){
        mexErrMsgTxt("Expecting at least 2 parameters\n");
    }
    
    if (!mxIsSparse(prhs[0])){
        mexErrMsgTxt("expecting sparse matrix for the 1st parameter\n");
    }
    
    
    if (!mxIsDouble(prhs[0])){
        mexErrMsgTxt("The matrix must be double\n");
    }
    
    if (!mxIsDouble(prhs[1])){
        mexErrMsgTxt("The 2nd parameter should be the number of clusters\n");
    }
    
    if (nrhs >=3){
      MAX_ITER = mxGetScalar(prhs[2]);   
    }
    
    if (nrhs >=4){
     TOL = mxGetScalar(prhs[3]);   
    }
    
    SparseMatrix data = SparseMatrix(prhs[0]);
    int k = mxGetScalar(prhs[1]);
    
    mainCluster(data, k, plhs);
    //     for (int i=0; i<20; i++){
    //        mexPrintf("%d, %d\n", generateRandom(100, 1), RAND_MAX);
    //    }
}

