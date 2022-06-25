import java.util.ArrayList;

class RW {
    private int reader, writer;

    public RW() {
        this.reader = 0;
        this.writer = 0;
    }

    public synchronized void ReaderEntry(int id) {
        try {
            while(this.writer > 0) {
                System.out.println("rw.ReaderBlocked(" + id + ")");
                wait();
            }
                this.reader++;
                System.out.println("rw.ReaderReading("+id+")");
        } catch(InterruptedException e) {}
    }

    public synchronized void ReaderExiting(int id) {
        this.reader--;
        if(this.reader == 0)
            this.notify();
        System.out.println("rw.ReaderExiting("+id+")");
    }

    public synchronized void WriterEntry(int id) {
        try {
            while((this.reader >0) || (this.writer > 0)) {
                System.out.println("rw.WriterBlocked("+id+")");
                wait();
            }
            this.writer++;
            System.out.println("rw.WriterWriting("+id+")");
        } catch(InterruptedException e) {}
    }

    public synchronized void WriterExiting(int id) {
        this.writer--;
        notifyAll();
        System.out.println("rw.WriterExiting("+id+")");
    }
}


class Structure {
    private int i;

    public Structure() { this.i = 0; }

    public synchronized void inc() { this.i++; }
    public synchronized void set(int i) { this.i = i; }
    public synchronized int get() { return this.i; }
}

class T1 extends Thread {
    private int id;
    Structure s;
    RW monitor;

    public T1(int id, Structure s, RW monitor) {
        this.id = id;
        this.s = s;
        this.monitor = monitor;
    }

    public void run() {
        this.monitor.WriterEntry(this.id);
        s.inc();
        this.monitor.WriterExiting(this.id);
    }
}

class T2 extends Thread {
    private int id;
    Structure s;
    RW monitor;

    public T2(int id, Structure s, RW monitor) {
        this.id = id;
        this.s = s;
        this.monitor = monitor;
    }

    public void run() {
        this.monitor.ReaderEntry(this.id);
        if(s.get() % 2 == 0)
            System.out.println("T2: Par");
        else
            System.out.println("T2: Ímpar");
        this.monitor.ReaderExiting(this.id);
    }
}

class T3 extends Thread {
    private int id;
    Structure s;
    RW monitor;

    public T3(int id, Structure s, RW monitor) {
        this.id = id;
        this.s = s;
        this.monitor = monitor;
    }

    public void run() {
        this.monitor.ReaderEntry(this.id);
        System.out.println("T3: " + s.get());
        for(int i=0; i<100000;)
            i++;
        this.monitor.ReaderExiting(this.id);

        this.monitor.WriterEntry(this.id);
        s.set(id);
        this.monitor.WriterExiting(this.id);
    }
}

public class Main {
    private static final int R = 4;
    private static final int W = 3;
    private static final int lenT1 = 1, lenT2 = 2, lenT3 = 1;

    public static void main(String[] args) {
        Structure s = new Structure();
        RW monitor = new RW();
        ArrayList<T1> t1ArrayList = new ArrayList<>();
        ArrayList<T2> t2ArrayList = new ArrayList<>();
        ArrayList<T3> t3ArrayList = new ArrayList<>();

        for(int i=0; i<lenT1; i++) {
            t1ArrayList.add(new T1(i+1, s, monitor));
            t1ArrayList.get(i).start();
        }
        for(int i=0; i<lenT2; i++) {
            t2ArrayList.add(new T2(i+1, s, monitor));
            t2ArrayList.get(i).start();
        }
        for(int i=0; i<lenT3; i++) {
            t3ArrayList.add(new T3(i+1, s, monitor));
            t3ArrayList.get(i).start();
        }
    }
}
