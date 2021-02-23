package task1;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;

public class MyRunnable implements Runnable {
    int destination;
    ExecutorService ex;
    ArrayList<Integer> path;

    MyRunnable(int destination, ExecutorService ex, ArrayList<Integer> path) {
        this.destination = destination;
        this.ex = ex;
        this.path = path;
    }

    @Override
    public void run() {
        int lastNodeInPath = path.get(path.size() - 1);
        if (lastNodeInPath == this.destination) {
            System.out.println(path);
            ex.shutdown();
            return;
        }

        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (path.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(path);
                newPartialPath.add(ints[1]);
                ex.submit(new MyRunnable(destination, ex, newPartialPath));
            }
        }
    }
}
