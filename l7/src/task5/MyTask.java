package task5;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class MyTask extends RecursiveTask<Void> {

    int[] colors;
    int step;
    ForkJoinPool fjp;

    MyTask(int[] colors, int step, ForkJoinPool fjp) {
        this.colors = colors;
        this.step = step;
        this.fjp = fjp;
    }

    private static boolean verifyColors(int[] colors, int step) {
        for (int i = 0; i < step; i++) {
            if (colors[i] == colors[step] && isEdge(i, step))
                return false;
        }
        return true;
    }

    private static boolean isEdge(int a, int b) {
        for (int[] ints : Main.graph) {
            if (ints[0] == a && ints[1] == b)
                return true;
        }
        return false;
    }

    static void printColors(int[] colors) {
        StringBuilder aux = new StringBuilder();
        for (int color : colors) {
            aux.append(color).append(" ");
        }
        System.out.println(aux);
    }

    @Override
    protected Void compute() {

        if (this.step == Main.N) {
            printColors(colors);
            return null;
        }
        List<MyTask> tasks = new ArrayList<>();
        // for the node at position step try all possible colors
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = this.colors.clone();
            newColors[this.step] = i;
            if (verifyColors(newColors, this.step)) {
                MyTask t = new MyTask(newColors, this.step + 1, fjp);
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
