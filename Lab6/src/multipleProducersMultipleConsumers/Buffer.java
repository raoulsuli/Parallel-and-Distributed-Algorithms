package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	ArrayBlockingQueue<Integer> queue = new ArrayBlockingQueue<>(10);

	void put(int value) throws InterruptedException {
		queue.put(value);
	}

	int get() throws InterruptedException {
		return queue.take();
	}
}
