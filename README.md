we have five different graphs in file folders

Each folder contains three files



- edge.rsf：it's edge file

```
depends alias.c alias.h
```

it means that the edge is  alias.c<-alias.h

- groundtruth.rsf：it's label file

```
contain general alias.c
```

it means that the node of  alias.c's label is 'general'

- new_filevectors.json：it is a json file that contains data.x

  ```
  {"alias.c": [-0.43996699999999994, 0.4523440000000001, -0.07441, 0.416301,...  0.640687, -0.278378, -0.32747499999999996],
  ```

  it means that the node of  alias.c's x is [-0.43996699999999994, 0.4523440000000001, -0.07441, 0.416301,...  0.640687, -0.278378, -0.32747499999999996]
  Some of these nodes are not in the label file, we group them into one category by default.
