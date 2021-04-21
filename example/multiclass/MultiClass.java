package multiclass;

class MultiClass {

    public static void main(String[] args) {
        MultiHello hello = new MultiHello("world");
        hello.greet();
        hello.setMessage("all");
        hello.greet();
    }
}
