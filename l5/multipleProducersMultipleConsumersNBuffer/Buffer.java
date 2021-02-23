package multipleProducersMultipleConsumersNBuffer;

import java.util.concurrent.Semaphore;

/**
 * @author Gabriel Gutu <gabriel.gutu at upb.ro>
 *
 */
public class Buffer {
    
    LimitedQueue queue;
    Semaphore plin, gol;
    
    public Buffer(int size) {
        queue = new LimitedQueue(size);
        plin = new Semaphore(0);
        gol = new Semaphore(size);
    }

	void put(int value) throws InterruptedException {
        gol.acquire();
        synchronized (queue) {
            queue.add(value);       
        }
        plin.release();
	}

	int get() throws InterruptedException {
        plin.acquire();
        int item;
        synchronized (queue) {
            item = (int) queue.poll();
        }
        gol.release();
        return item;
	}
}
