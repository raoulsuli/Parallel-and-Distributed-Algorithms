package shortestPathsFloyd_Warshall;
/**
 * @author cristian.chilipirea
 *
 */
public class Main {

	public static void main(String[] args) {
		int M = 9;
		int graph[][] = { { 0, 1, M, M, M }, 
				          { 1, 0, 1, M, M }, 
				          { M, 1, 0, 1, 1 }, 
				          { M, M, 1, 0, M },
						  { M, M, 1, M, 0 } };

		int cores = Runtime.getRuntime().availableProcessors();
		MyThread[] threads = new MyThread[cores];

		for (int i = 0; i < cores; i++) {
			threads[i] = new MyThread(i, graph.length, cores, graph);
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

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				System.out.print(graph[i][j] + " ");
			}
			System.out.println();
		}
	}
}
