package multiclass;

class MultiHello {
    private String msg;

    MultiHello(String m) {
        this.msg = m;
    }

    void setMessage(String m) {
        this.msg = m;
    }

    void greet() {
        System.out.println("Hello " + msg);
    }
}
