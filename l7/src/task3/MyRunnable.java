package task3;

import java.util.concurrent.ExecutorService;

public class MyRunnable implements Runnable{
    int[] graph;
    int step;
    ExecutorService tpe;

    MyRunnable(int[] graph, int step, ExecutorService tpe) {
        this.graph = graph;
        this.step = step;
        this.tpe = tpe;
    }

    private static boolean check(int[] arr, int step) {
        for (int i = 0; i <= step; i++) {
            for (int j = i + 1; j <= step; j++) {
                if (arr[i] == arr[j] || arr[i] + i == arr[j] + j || arr[i] + j == arr[j] + i)
                    return false;
            }
        }
        return true;
    }

    private static void printQueens(int[] sol) {
        StringBuilder aux = new StringBuilder();
        for (int i = 0; i < sol.length; i++) {
            aux.append("(").append(sol[i] + 1).append(", ").append(i + 1).append("), ");
        }
        aux = new StringBuilder(aux.substring(0, aux.length() - 2));
        System.out.println("[" + aux + "]");
    }

    @Override
    public void run() {
        if (Main.N == this.step) {
            printQueens(this.graph);
            tpe.shutdown();
            return;
        }
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = this.graph.clone();
            newGraph[this.step] = i;

            if (check(newGraph, this.step)) {
                tpe.submit(new MyRunnable(newGraph, step + 1, tpe));
            }
        }
    }
}
