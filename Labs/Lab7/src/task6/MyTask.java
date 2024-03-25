package task6;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class MyTask extends RecursiveTask<Void> {

    int[] graph;
    int step;
    ForkJoinPool fjp;

    MyTask(int[] graph, int step, ForkJoinPool fjp) {
        this.graph = graph;
        this.step = step;
        this.fjp = fjp;
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
    protected Void compute() {

        if (Main.N == step) {
            printQueens(graph);
            return null;
        }

        List<MyTask> tasks = new ArrayList<>();

        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                MyTask t = new MyTask(newGraph, step + 1, fjp);
                tasks.add(t);
                t.fork();
            }
        }

        for (MyTask task: tasks) {
            task.join();
        }

        return null;
    }
}
