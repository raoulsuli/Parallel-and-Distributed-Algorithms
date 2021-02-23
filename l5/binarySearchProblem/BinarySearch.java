package binarySearchProblem;

public class BinarySearch implements Runnable{
    int start, end;
    int[] array;
    int target;

    public BinarySearch(int[] array, int start, int end, int target) {
        this.start = start;
        this.end = end;
        this.array = array;
        this.target = target;
    }

    @Override
    public void run() {
        synchronized (array) {
            if (array[start] > target || array[end] < target) {
                return;
            } else if (target == array[start]) {
                System.out.println("Item found at " + start);
                return;
            } else if (target == array[end]) {
                System.out.println("Item found at " + end);
                return;
            } else {
                int mid = (start + end) / 2;
                while (start < end) {
                    if (array[mid] == target) {
                        System.out.println("Item found at " + mid);
                        return;
                    } else if (array[mid] > target) {
                        end = mid;
                    } else {
                        start = mid;
                    }
                }
            }
        }
    }


}
