# Outro Sort

A hybrid sorting algorithm which leverages the power of multithreading. The current implementation works only on
integers. It is faster than the `std::sort` function of the C++ standard library.

| Array Size     | `std::sort` | `outro_sort` |
| :------------: | ----------: | -----------: |
| 2<sup>15</sup> | 4 ms        | 5 ms         |
| 2<sup>16</sup> | 9 ms        | 7 ms         |
| 2<sup>17</sup> | 19 ms       | 9 ms         |
| 2<sup>18</sup> | 40 ms       | 14 ms        |
| 2<sup>19</sup> | 84 ms       | 30 ms        |
| 2<sup>20</sup> | 178 ms      | 44 ms        |
| 2<sup>21</sup> | 371 ms      | 84 ms        |
| 2<sup>22</sup> | 765 ms      | 171 ms       |
| 2<sup>23</sup> | 1627 ms     | 366 ms       |
| 2<sup>24</sup> | 3330 ms     | 714 ms       |

These are the running times reported on my 4C/8T for random arrays.
