/* Linear regression line from n points with coords (x, y). a = intercept, b = gradient */
void sse_line(int n, double *x, double *y, double *a, double *b)
{
    double ybar = 0.0, xbar = 0.0, num = 0.0, den = 0.0, sumy = 0.0, sumx = 0.0;
    for (int i = 0; i < n; i++)
    {
        sumy += y[i];
        sumx += x[i];
    }
    ybar = sumy / (double)n;
    xbar = sumx / (double)n;
    for (int i = 0; i < n; i++)
    {
        num += (x[i] - xbar) * (y[i] - ybar);
        den += pow(x[i] - xbar, 2);
    }
    *b = num / den;
    *a = ybar - *b * xbar;
}

void main(void)
{

}