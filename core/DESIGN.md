# Language structure

```rust (just for highlighting)
fn main() {
    factorial(5);
}

fn factorial(x) {
    if x == 1 {
        return 1;
    } else {
        return (x * factorial(x - 1));
    }
}
```
