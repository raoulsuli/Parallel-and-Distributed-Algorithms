package doubleVectorElements;

public class MyThread extends Thread{
    
    int id;
    int N;
    int cores;
    int[] v;

    MyThread(int id, int N, int cores,int[] v) {
        this.id = id;
        this.N = N;
        this.cores = cores;
        this.v = v;
    }

    public void run() {
        int start = (int)(id * (double) N / cores);
        int end = (int)Math.min(((id + 1) * (double) N / cores), N);

		for (int i = start; i < end; i++) {
			v[i] = v[i] * 2;
		}
    }
}
