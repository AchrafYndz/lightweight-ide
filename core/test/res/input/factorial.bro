fn main() {
    factorial(5);
}

// Returns the factorial of the given integer.
fn factorial(x) {
    if x == 1 {
        return 1;
    } else {
        return (x * factorial(x - 1));
    }
}
