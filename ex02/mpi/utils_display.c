/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_display.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurele <aurele@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 00:27:49 by aurele            #+#    #+#             */
/*   Updated: 2025/12/19 22:41:28 by aurele           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

/* Return whether (i,j) is on a stored diagonal and write its value. */
static int	tri_cell(t_vector *l, t_vector *d, t_vector *u, int i, int j, double *v)
{
    int n;

    n = d->size;
    if (j == i)
        return (*v = d->vec[i], 1);
    if (j == i - 1 && i > 0 && l && l->vec && (l->size == n - 1 || l->size == n))
        return (*v = (l->size == n - 1) ? l->vec[i - 1] : l->vec[i], 1);
    if (j == i + 1 && i < n - 1 && u && u->vec && (u->size == n - 1 || u->size == n))
        return (*v = u->vec[i], 1);
    return (0);
}

/* Print a vector on one line with a label. */
void    display_vector(t_vector *vector, const char *name)
{
    printf("%s: ", name);
    int i = 0;
    while (i < vector->size)
    {
        printf("%f ", vector->vec[i]);
        i++;
    }
    printf("\n");
}

/* Pretty-print a tridiagonal matrix using '.' for empty cells. */
void    display_tridiag_matrix(t_vector *lower, t_vector *diag, t_vector *upper,
            const char *name)
{
    int n;
    int i;
    int j;
    double v;

    if (!diag || !diag->vec || diag->size <= 0)
        return ;
    n = diag->size;
    if (name)
        printf("%s\n", name);
    i = 0;
    while (i < n)
    {
        j = 0;
        while (j < n)
        {
            if (tri_cell(lower, diag, upper, i, j, &v))
                printf("%8.2f", v);
            else
                printf("%8s", ".");
            j++;
        }
        printf("\n");
        i++;
    }
}