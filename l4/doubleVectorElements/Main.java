package doubleVectorElements;
/**
 * @author cristian.chilipirea
 *
 */
public class Main {

	public static void main(String[] args) {
		int N = 100000013;
		int v[] = new int[N];
		int cores = Runtime.getRuntime().availableProcessors();
		MyThread[] threads = new MyThread[cores];

		for(int i=0;i<N;i++)
			v[i]=i;

		for (int i = 0; i < cores; i++) {
			threads[i] = new MyThread(i, N, cores, v);
		}

		for (int i = 0; i < cores; i++) {
			threads[i].start();
		}

		for (int i = 0; i < cores; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		for (int i = 0; i < N; i++) {
			if(v[i]!= i*2) {
				System.out.println("Wrong answer");
				System.exit(1);
			}
		}
		System.out.println("Correct");
	}

}
