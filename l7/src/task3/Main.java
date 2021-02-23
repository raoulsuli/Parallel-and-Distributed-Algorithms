package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    public static int N = 4;

    public static void main(String[] args) {
        int[] graph = new int[N];
        ExecutorService tpe = Executors.newFixedThreadPool(4);
        tpe.submit(new MyRunnable(graph, 0, tpe));
    }
}
