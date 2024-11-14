Lamprou Konstantinos-Georgios


### Documentation

This project implements a hybrid hashing technique combining open addressing with linear probing and separate chaining. The approach optimizes the use of neighboring slots in open addressing while managing collisions effectively through separate chaining.

Complexity
Worst-Case: Both insert and search operations have a complexity of O(n).
Average-Case: For both insert and search, the complexity is O(1) on average.
Real-Time and Amortized Complexity: Both operations maintain O(1) complexity in real-time and amortized scenarios.

Comparison to Classic Open Addressing
While the hybrid method shares the same average and amortized complexity with classic open addressing, it improves worst-case performance by using separate chaining, reducing the impact of clustering.



