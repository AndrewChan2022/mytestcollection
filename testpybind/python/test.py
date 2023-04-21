import mylibexample

print("print_vector:", mylibexample.print_vector([1, 2, 3]))

print("mylibadd(1,2) = ", mylibexample.mylibadd(1,2))

m = mylibexample.MyClass()
m.contents = [1, 2, 3]
m.contents = mylibexample.VectorInt([1, 2, 3])
m.items = [mylibexample.MyItem(0,0), mylibexample.MyItem(1,1), mylibexample.MyItem(2,2)]
m.items.append(mylibexample.MyItem(3,3))
print(m)

print(m.items[0])

m.items.clear()
print("m after clear:", m)



# m.contents.append(1)
# m.contents.append(2)
# m.contents.append(3)
# print()
# print(m)

# mylibexample.print_vector([1, 2, 3])