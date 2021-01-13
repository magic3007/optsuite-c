#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File              : logistic_regression.py
# Author            : Jing Mai <jingmai@pku.edu.cn>
# Date              : 01.13.2021
# Last Modified Date: 01.13.2021
# Last Modified By  : Jing Mai <jingmai@pku.edu.cn>
import torch


def main():
    m, n = 512, 256
    dtype = torch.float64
    A = torch.randn(n, m).to(dtype)
    b = torch.randn(m).to(dtype)
    x = torch.randn(n).to(dtype)
    x.requires_grad_(True)
    
    t = torch.zeros(m)
    
    for i in range(m):
        t[i] = torch.log1p(torch.exp(-b[i] * torch.dot(A[:, i], x)))
    y = t.mean()
    print(y.item())
    y.backward()
    print(torch.norm(x.grad, p=2).item())


if __name__ == '__main__':
    main()
