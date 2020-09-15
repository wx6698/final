#!/usr/bin/env bash
aws eks --region us-east-1 update-kubeconfig --name eks-green
kubectl apply -f ./blue-controller.json
kubectl apply -f ./blue-green-service.json
kubectl get services
