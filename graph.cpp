


class Arc
{
  public:
    int neighbor; /* je garde cet attribut car on ne sait jamais */
    float flow;
    float capacity;
    float residualCapacity;

    Arc(int neighbor,float capacity);
};
