# Turing Machines — Exam Prep

This file covers the core theory of Turing Machines across five areas.

---

A Turing Machine is a theoretical model of computation. It has an infinite tape divided into cells, a head that reads and writes symbols, and a finite set of states. At each step, the machine reads the current symbol, writes a new one, moves the head left or right, and transitions to a new state. What makes it powerful is that this simple mechanism can compute anything that any modern computer can compute.

Languages are classified by what kind of machine can recognise them. Regular languages need only a finite automaton. Context-free languages need a pushdown automaton. Recursively enumerable languages need a Turing Machine that might loop forever on some inputs. Recursive languages need a Turing Machine that always halts with a clear yes or no answer. Everything outside the recursively enumerable class cannot be recognised by any machine at all.

Non-deterministic Turing Machines can branch into multiple computation paths at once and accept if any one of those paths leads to an accept state. They are not more powerful than deterministic machines in terms of what languages they can decide, but simulating one with a deterministic machine requires exploring the computation tree level by level rather than branch by branch, because going depth-first risks getting stuck in an infinite loop on one branch while a valid accepting path goes unexplored.

Decidability is about whether a problem can be solved by a machine that always gives an answer. The most important undecidable problem is the Acceptance Problem: given a Turing Machine and an input, does the machine accept that input? There is no algorithm that can correctly answer this for all cases. The proof works by assuming such an algorithm exists and constructing a machine that contradicts itself when run on its own description. The Halting Problem is similarly undecidable, shown by reducing the Acceptance Problem to it.

Reductions are a way of comparing the difficulty of problems. If you can transform any instance of problem A into an instance of problem B in a way that preserves the yes or no answer, then A reduces to B. This means B is at least as hard as A. To show a new problem is undecidable, you take a known undecidable problem and reduce it to the new one.

Rice's Theorem says that any non-trivial question about what language a Turing Machine accepts is undecidable. It does not matter what the question is, as long as the answer depends on the language and not on the internal mechanics of the machine. Questions like whether the machine accepts any strings at all, whether its language is finite, or whether it accepts strings of a particular form are all undecidable by this theorem.

The recursive languages are closed under every standard operation: union, intersection, complement, concatenation, and Kleene star. The recursively enumerable languages are closed under most of those but not complementation. A language is recursive if and only if both it and its complement are recursively enumerable, because you can run recognisers for both in parallel and one of them will always halt.
