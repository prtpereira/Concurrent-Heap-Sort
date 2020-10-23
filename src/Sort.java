import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


class HeapSort
{
    private Lock[] l;
    private Condition c;
    private boolean rootHeapified;
    static volatile Integer end;
    private int a[];

    public int[] get() {
        return a;
    }

    void siftDown(int start)
    {
        //System.out.printf("thread[%d]: siftDoen start a[%d]=%d with last a[%d]=%d\n", Thread.currentThread().getId(), start, a[start], end, a[end]);
        int root = start;
        l[root].lock();
        while( (2*root+1) <= end)
        {
            int child_l = 2*root+1;
            int child_r = child_l+1;
            int swap = root;

            l[child_l].lock();
            if(child_r <= end)
            {
                l[child_r].lock();
            }

            if (a[swap]<a[child_l]) swap = child_l;
            if ((child_r) <= end && a[swap]<a[child_r]) swap = child_r;
            if(swap==root)
            {
                l[child_l].unlock();
                if(child_r <= end) l[child_r].unlock();
                break;
            }
            else
            {
                synchronized(end)
                {
                    if(swap<=end) {int tmp = a[swap]; a[swap]=a[root]; a[root]=tmp;}
                }
                if(root == 0)
                {
                    rootHeapified = true;
                    c.signal();
                }
                l[root].unlock();
                if(swap == child_r) l[child_l].unlock();
                else if(child_r <= end) l[child_r].unlock();
                root = swap;
            }
        }
        if(root == 0)
        {
            rootHeapified = true;
            c.signalAll();
        }
        l[root].unlock();
        //System.out.printf("thread[%d]: siftDown end start a[%d]=%d \n", Thread.currentThread().getId(), start, a[start], end, a[end]);

    }

    void heapify()
    {
        int start = end/2;
        while(start >= 0) {
            siftDown(start);
            start--;
        }
    }

    public HeapSort(int[] a, int n_threads) throws Exception
    {
        this.a = a;
        ExecutorService pool = Executors.newFixedThreadPool(n_threads);
        l = new Lock[a.length];
        for(int i=0; i<a.length; i++) l[i] = new ReentrantLock();
        c = l[0].newCondition();
        rootHeapified = true;

        this.end=a.length-1;
        heapify();

        while (end > 0)
        {
            l[0].lock();
            try
            {
                while (rootHeapified == false) c.await();
                //System.out.printf("thread[root]: swaping root %d with last %d\n", a[0], a[end]);
                synchronized (end) {
                    int cmax = a[0]; a[0] = a[end]; a[end] = cmax;
                    this.end--;
                }
                rootHeapified = false;
            }
            finally
            {
                l[0].unlock();
            }
            //System.out.printf("thread[%d]: siftdown %d\n", id, a[0]);
            pool.execute( () -> { siftDown(0); } );
            //siftDown(0, end);
        }
        pool.shutdown();
    }

    public void print()
    {
        System.out.print("[");
        int i;
        for(i=0; i<a.length-1; ++i)
                System.out.print(a[i] + ", ");
        System.out.print(a[i] + "]\n");
    }

    public void test()
    {
        for(int i=0; i<a.length-1; ++i)
            if(a[i] > a[i+1])
            {
                System.out.print("Invalid Sort\n");
                return;
            }
        System.out.print("Valid Sort\n");
    }
}

public class Sort {
    public static void main(String[] args) throws Exception {
        int length = 10000;
        long seed = System.currentTimeMillis();
        int n_threads = 1;
        switch(args.length)
        {
            case 3: seed      = Integer.parseInt(args[2]);
            case 2: length    = Integer.parseInt(args[1]);
            case 1: n_threads = Integer.parseInt(args[0]);
                break;
            default: System.out.printf("usage sort: [n_threads] [length] [seed]\n"); return;
        }
        Random r = new Random(seed);

        int a[] = new int[length];
        for(int i=0; i<length; ++i) a[i] = r.nextInt(50);

        long start = System.currentTimeMillis();
        HeapSort hs = new HeapSort(a, n_threads);
        long time = System.currentTimeMillis() - start;
        System.out.println(time);
        hs.test();
        //hs.print();
    }
}
