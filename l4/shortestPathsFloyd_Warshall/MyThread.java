package shortestPathsFloyd_Warshall;

public class MyThread extends Thread{
    
    int id;
    int N;
    int cores;
    int[][] graph;

    MyThread(int id, int N, int cores,int[][] graph) {
        this.id = id;
        this.N = N;
        this.cores = cores;
        this.graph = graph;
    }

    public void run() {
        int start = (int)(id * (double) N / cores);
        int end = (int)Math.min(((id + 1) * (double) N / cores), N);

		for (int k = 0; k < N; k++) {
			for (int i = start; i < end; i++) {
				for (int j = 0; j < N; j++) {
					graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
				}
			}
		}
    }
}
