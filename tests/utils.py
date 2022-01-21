import re
from collections import defaultdict
from typing import List

import numpy


def geomean(a):
    return numpy.exp(numpy.log(a).sum() / len(a))


def invert_table(table: List[List[str]]) -> List[List[str]]:
    new_table = [[] for _ in table[0]]
    for row in table:
        for new_row, field in zip(new_table, row):
            new_row.append(field)
    return new_table


def draw_table(table: List[List[str]], hline_after=()):
    column_width = defaultdict(lambda: 0)
    for row in table:
        for i, col in enumerate(row):
            column_width[i] = max(column_width[i], len(col))
    txt = []
    for i, row in enumerate(table):
        for j, col in enumerate(row):
            txt.append(col + ' ' * (column_width[j] - len(col)))
            if j < len(row) - 1:
                txt.append(' | ')
        txt.append('\n')
        if i in hline_after:
            # txt.append('-' * (sum(column_width.values()) + 3 * len(row) - 3) + '\n')
            txt.append('-|-'.join('-' * v for k, v in sorted(column_width.items(), key=lambda x: x[0])) + '\n')
    return ''.join(txt)


def format_tex_string_for_table(s: str) -> str:
    if re.search(r'^__\d+__ ', s):
        s = s.split(' ', 1)[1]
    s = s.replace('#', '\\#').replace('_', '\\_').replace('%', '\\%')
    if '\n' in s:
        s = '\\makecell{' + s.replace('\n', '\\\\') + '}'
    return s


def draw_table_tex(table: List[List[str]], hline_after=()):
    column_width = defaultdict(lambda: 0)
    for row in table:
        for i, col in enumerate(row):
            col = format_tex_string_for_table(col)
            column_width[i] = max(column_width[i], len(col))
    txt = ['\\begin{tabular}{|' + 'c|' * len(table[0]) + '}\n\\hline\n']
    for i, row in enumerate(table):
        for j, col in enumerate(row):
            col = format_tex_string_for_table(col)
            txt.append(col + ' ' * (column_width[j] - len(col)))
            if j < len(row) - 1:
                txt.append(' & ')
        txt.append('\\\\\n')
        if i in hline_after:
            txt.append('\\hline\n')
    txt.append('\\hline\n\\end{tabular}')
    return ''.join(txt)
