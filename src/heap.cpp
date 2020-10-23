#include <iostream>       // std::cout
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
//#include <cprocess>

std::mutex *nodes;
int *tids;
int ths;


int isSorted(int a[], int n, int *k) {
  for(int i=0; i<n-1; i++) {
    if (a[i]>a[i+1]) {*k=i; return 0;}
  }
  return 1;
}

void shiftDownSeq(int a[], int start, int end) {
  int root = start;
  while( (2*root+1) <= end) {
    int child = 2*root+1;
    int swap = root;
    if(a[swap]<a[child]) swap = child;
    if((child+1) <= end)
      if(a[swap]<a[child+1]) swap = child+1;
    if(swap==root) {
      root=end; // return
    }
    else {
      int tmp = a[swap]; a[swap]=a[root]; a[root]=tmp;
      root = swap;
      //unlock root
    }
  }
}

void heapify(int a[], int count) {

    int start=(count-1)/2;
    while(start>=0) {
      shiftDownSeq(a,start,count-1);
      start--;
    }
}

void heapsortSeq(int a[], int count, int nthreads) {

  heapify(a,count); // build max-heap(sequential)
  int end=count-1;
  while( end>0) { // do in parallel
    // do in sequential order
    int cmax= a[0]; a[0]=a[end]; a[end]=cmax; //swap
    end--; // decrease heap size
    // repair heap : do with locking of parent and child
    shiftDownSeq(a,0,end);
  }

}



std::mutex access;
/*volatile*/ std::atomic<int> endH; //variavel 'end' atomica, thread pode estar a meio do bubble e um elemento ser retirado

void shiftDown(int a[], int start) {

  nodes[start].lock();
  nodes[endH].lock();
    int cmax= a[0]; a[0]=a[endH]; a[endH]=cmax; //swap
    endH--; // decrease heap size
  nodes[endH+1].unlock();
  nodes[start].unlock();

  int root = start;
  while( (2*root+1) <= endH) {

    nodes[root].lock();
    int rootx=root;
    int child = 2*root+1;

    int esq = child;
    int dir = child+1;

    //lock childs
    nodes[esq].lock();
    nodes[dir].lock();

    int swap = root;
    if(a[swap]<a[child]) swap = child;
    if((child+1) <= endH)
      if(a[swap]<a[child+1]) swap = child+1;

    if(swap==root) {
      root=endH; // return
    }
    else {
      int tmp=a[swap]; a[swap]=a[root]; a[root]=tmp;
      root = swap;
    }

    //unlock childs
    nodes[dir].unlock();
    nodes[esq].unlock();

    nodes[rootx].unlock();
  }

}


//---------skeleton code for parallel version with locks
void heapsort(int a[], int count, int nthreads) {

  heapify(a,count); // build max-heap(sequential)
  std::thread threads[count];
  endH=count-1;

  int i=0;
  while( i<count-1 ) {

    threads[i]=std::thread (shiftDown, a, 0);
    i++;
  }

  // 0 8
  for(int i=0; i<count-1; i++) {
    if(threads[i].joinable()) threads[i].join();
  }

}



int main(int argc, char* argv[]) {

  int size = 50;
  int begin=1;
  int end;
  int threads=-1;

  if (argc==1) {
    std::cout << "Usage:  " << argv[0] << " <size>" << " <n_threads>\n";
    return -1;
  }

  if (argc>1) size=atoi(argv[1]);
  if (argc>2) threads=atoi(argv[2]);
  end=size*1000;

  int nums[size];
  nodes=new std::mutex[size];

  srand((unsigned)time(NULL));
  for(int i=0; i<size; i++) {
    nums[i] = begin+(rand()%end);
    // std::cout << nums[i] << " ";
  }// std::cout << "\n";


  // std::chrono::time_point<std::chrono::system_clock> startseq = std::chrono::system_clock::now();
  // heapsortSeq(nums, size, threads);
  // std::chrono::duration<double, std::milli> ttimeseq = (std::chrono::system_clock::now() - startseq);


  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  heapsort(nums, size, threads);
  std::chrono::duration<double, std::milli> ttime = (std::chrono::system_clock::now() - start);

  //
  // for(int i : nums)
  //   std::cout << i << " ";
  // std::cout << "\n";

  std::cout << /*"Seq: " << ttimeseq.count() << "\nPar: " <<*/ ttime.count() << std::endl;

  // int idx=-1;
  // if (isSorted(nums, size, &idx))
  //   std::cout << "[ V ] Is Sorted!!" << std::endl;
  // else std::cout << "[ X ] NOT Sorted!!! " << idx << std::endl;

  delete[] nodes;

  return 0;
}
