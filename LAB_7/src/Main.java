import java.util.ArrayList;
import java.util.Random;

class EvenCounter extends Thread {
    private int count;
    private final int start, end;
    private final ArrayList<Integer> arr;

    public EvenCounter(int start, int end, ArrayList<Integer> arr) {
        this.count = 0;
        this.start = start;
        this.end = end;
        this.arr = arr;
    }

    public void run() {
        for(int i=this.start; i<this.end; i++)
            if(this.arr.get(i) % 2 == 0)
                this.count++;
    }

    public int getCount() {
        return this.count;
    }
}

public class Main {
    private static int evenCount(ArrayList<Integer> arr) {
        int count = 0;
        for(var e : arr)
            if(e % 2 == 0)
                count++;
        return count;
    }

    public static void main(String[] args) throws InterruptedException {
        final var MAX = 99999;
        final var SIZE = 999999;
        final var NTHREADS = 4;
        final var BLOCKSIZE = SIZE / NTHREADS;

        var arr = new ArrayList<Integer>(SIZE);
        var threads = new ArrayList<EvenCounter>(NTHREADS);
        var rand = new Random();

        // preenchendo o array com numeros aleatorios
        for(int i=0; i<SIZE; i++)
            arr.add(rand.nextInt(MAX));

        for(int i=0; i<NTHREADS; i++) {
            int start = i * BLOCKSIZE;
            int end = (i == NTHREADS-1 ? SIZE : start + BLOCKSIZE);
            threads.add(new EvenCounter(start, end, arr));
            threads.get(i).start();
        }

        int evenCountC = 0;
        for(var t : threads) {
            t.join();
            evenCountC += t.getCount();
        }

        int evenCountS = evenCount(arr);

        System.out.println("Count with threads: " + evenCountC);
        System.out.println("Sequential count: " + evenCountS);

        if(evenCountS != evenCountC)
            System.out.println("An error occurred with a thread.");
    }
}