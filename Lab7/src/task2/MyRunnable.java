package task2;

import java.util.concurrent.ExecutorService;

public class MyRunnable implements Runnable{
    int[] colors;
    int step;
    ExecutorService ex;

    MyRunnable(int[] colors, int step, ExecutorService ex) {
        this.colors = colors;
        this.step = step;
        this.ex = ex;
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
    public void run() {
        if (this.step == Main.N) {
            printColors(this.colors);
            ex.shutdown();
            return;
        }

        // for the node at position step try all possible colors
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[this.step] = i;
            if (verifyColors(newColors, step))
                this.ex.submit(new MyRunnable(newColors, step + 1, ex));
        }

    }
}
