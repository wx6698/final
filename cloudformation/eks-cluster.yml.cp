---
AWSTemplateFormatVersion: '2010-09-09'
Description: 'EKS Cluster definition.'

Parameters:

  VpcId:
    Type: String
    Description: VPC ID.

  Subnets:
    Type: CommaDelimitedList
    Description: List of subnets in the VPC.

  ClusterName:
    Type: String
    Description: EKS Kubernetes cluster name.

  KubernetesVersion:
    Type: String
    Description: EKS Kubernetes cluster version.

  EnvironmentName:
    Description: An environment name that will be prefixed to resource names
    Type: String
    Default: final-green


Resources:

  ClusterRole:
    Type: AWS::IAM::Role
    Properties:
      AssumeRolePolicyDocument:
        Version: 2012-10-17
        Statement:
          Effect: Allow
          Principal:
            Service:
              - eks.amazonaws.com
          Action: sts:AssumeRole
      ManagedPolicyArns:
        - arn:aws:iam::aws:policy/AmazonEKSClusterPolicy
        - arn:aws:iam::aws:policy/AmazonEKSServicePolicy

  ClusterControlPlaneSecurityGroup:
    Type: AWS::EC2::SecurityGroup
    Properties:
      GroupDescription: Cluster communication with worker nodes.
      VpcId: !Ref VpcId

  Cluster:
    Type: "AWS::EKS::Cluster"
    Properties:
      Name: !Ref ClusterName
      Version: !Ref KubernetesVersion
      RoleArn: !GetAtt ClusterRole.Arn
      ResourcesVpcConfig:
        SecurityGroupIds:
          - !Ref ClusterControlPlaneSecurityGroup
        SubnetIds: !Ref Subnets

  WebAppLB:
    Type: AWS::ElasticLoadBalancingV2::LoadBalancer
    Properties:
      Subnets: !Ref Subnets
      SecurityGroups:
      - Ref: ClusterControlPlaneSecurityGroup

  Listener:
    Type: AWS::ElasticLoadBalancingV2::Listener
    Properties:
      DefaultActions:
        - Type: forward
        TargetGroupArn:
          Ref: WebAppTargetGroup
      LoadBalancerArn:
        Ref: WebAppLB
      Port: '80'
      Protocol: HTTP

  ALBListenerRule:
    Type: AWS::ElasticLoadBalancingV2::ListenerRule
    Properties:
      Actions:
        - Type: forward
      TargetGroupArn: !Ref 'WebAppTargetGroup'
      Conditions:
        - Field: path-pattern
          Values: [/]
      ListenerArn: !Ref 'Listener'
      Priority: 1

  WebAppTargetGroup:
    Type: AWS::ElasticLoadBalancingV2::TargetGroup
    Properties:
      HealthCheckIntervalSeconds: 10
      HealthCheckPath: /
      HealthCheckProtocol: HTTP
      HealthCheckTimeoutSeconds: 8
      HealthyThresholdCount: 2
      Port: 80
      Protocol: HTTP
      UnhealthyThresholdCount: 5
      VpcId: !Ref VpcId


Outputs:

  ClusterName:
    Value: !Ref ClusterName
    Description: Cluster Name
    Export:
      Name:
        Fn::Sub: "${AWS::StackName}-ClusterName"

  ClusterEndpoint:
    Value: !GetAtt Cluster.Endpoint
    Description: Cluster Endpoint
    Export:
      Name:
        Fn::Sub: "${AWS::StackName}-ClusterEndpoint"
