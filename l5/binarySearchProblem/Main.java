package binarySearchProblem;

public class Main {
    public static final int N = 6;
    public static final int BUFFER_SIZE = 120;
    public static final int number_to_be_found = 118;

    public static void main(String[] args) {
		Thread threads[] = new Thread[N];
        System.out.println("Binary Search problem!");
        
        int[] array = new int[BUFFER_SIZE];
        int start = 0;
        int end = 0;
        int i = 0;

        for (i = 0; i < BUFFER_SIZE; i++) {
            array[i] = 20 + 2 * i;
        }
        for (i = 0; i < N; i++) {
            start = (int) (i * Math.ceil (BUFFER_SIZE / N));
            end = (int) Math.min((i + 1) * Math.ceil (BUFFER_SIZE / N), BUFFER_SIZE);
			threads[i] = new Thread(new BinarySearch(array, start, end, number_to_be_found));
        }
		for (i = 0; i < N; i++)
			threads[i].start();

		for (i = 0; i < N; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
