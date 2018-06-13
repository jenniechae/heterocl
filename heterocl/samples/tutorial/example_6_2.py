import heterocl as hcl
import numpy as np

def find_max2(val, acc):
  with hcl.if_(val > acc[0]):
    with hcl.if_(val < acc[1]):
      acc[0] = val
    with hcl.else_():
      acc[0] = acc[1]
      acc[1] = val

k = hcl.reduce_axis(0, 10, "k")
init = hcl.compute((2,), [], lambda x: 0, "init")
R = hcl.reducer(init, find_max2)

A = hcl.placeholder((10, 10), "A")
B = hcl.compute((2, 10), [A, init], lambda _, y: R(A[k, y], axis = k), "B")

s = hcl.create_schedule(B)

print s.sch.stages
print B.axis
print B.reducer.axis
s[B.reducer].compute_at(s[B], B.axis[1])
print hcl.lower(s, [A, B])
f = hcl.build(s, [A, B])

_A = hcl.asarray(np.random.randint(20, size = A.shape), hcl.Int())
_B = hcl.asarray(np.zeros(B.shape), hcl.Int())

f(_A, _B)

print _A
print _B
