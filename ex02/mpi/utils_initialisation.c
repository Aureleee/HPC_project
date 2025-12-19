/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_initialisation.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurele <aurele@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:06:06 by aurele            #+#    #+#             */
/*   Updated: 2025/12/19 22:41:37 by aurele           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

/* Initialize all vectors for a tridiagonal system A*x=b (and an empty result). */
void initialize_tridiag_system(t_vector *lower, t_vector *diag, t_vector *upper,
                                t_vector *b, t_vector *result,
                                int n, double lower_value, double diag_value,
                                double upper_value, double b_value)
{
    init_vector(lower, n, lower_value);
    init_vector(diag, n, diag_value);
    init_vector(upper, n, upper_value);
    init_vector(b, n, b_value);
    init_vector(result, n, 0);
}

/* Free all vectors used in the tridiagonal system. */
void free_tridiag_system(t_vector *lower, t_vector *diag, t_vector *upper,
                            t_vector *b, t_vector *result)
{
    free_vector(lower);
    free_vector(diag);
    free_vector(upper);
    free_vector(b);
    free_vector(result);
}

/* Parse CLI arguments into parameters (SIZE, LOWER, DIAG, UPPER, B, DISPLAY). */
void handle_input(int argc, char **argv, int *n, double *lower_value,
                    double *diag_value, double *upper_value, double *b_value,
                    int *display)
{
    if (argc >= 5)
    {
        *n = atoi(argv[1]);
        *lower_value = atof(argv[2]);
        *diag_value = atof(argv[3]);
        *upper_value = atof(argv[4]);
        if (*n <= 0)
            *n = 5;
    }
    if (argc >= 6 && b_value)
        *b_value = atof(argv[5]);
    if (argc >= 7 && display)
        *display = atoi(argv[6]);
}