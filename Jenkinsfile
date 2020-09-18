pipeline {
     agent any
     stages {
         stage('AWS Credentials') {
             steps {
                withCredentials([[$class: 'AmazonWebServicesCredentialsBinding', credentialsId: 'bg', accessKeyVariable: 'ACCESS_KEY', secretKeyVariable: 'SECRET_KEY']]){
                 sh '''
                     mkdir -p ~/.aws
                     echo "[default]" > ~/.aws/credentials
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.aws/credentials
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.aws/credentials
                     echo "[default]" > ~/.boto
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.boto
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.boto
                 '''
                }
             }
         }
         stage('Lint') {
              steps {
                  sh '''
                     echo "Validating Dockerfile and app python file..."
                     hadolint Dockerfile
                     pylint --disable=R,C,W1203 app.py
                     # Validate Cloudformation templates.
                     echo "Validating CloudFormation templates..."
                     for template in cloudformation/*.yml; do
                     yamllint --strict $template
                     cfn-lint --include-checks I --template $template
                     # validate-template requires valid AWS Credentials :(
                     # aws cloudformation validate-template --template-body file://$template --region us-east-1
                     done
                     # Check ansible playbooks for correctness.
                     playbooks="main.yml delete.yml"
                     echo "Checking playbook syntax..."
                     ansible-playbook -i inventory $playbooks --syntax-check
                     echo "Linting playbooks with ansible-lint..."
                     ansible-lint $playbooks
                     '''
              }
         }
        stage('Deploy EKS cluster') {
              steps {
                  sh  'ansible-playbook -i inventory main.yml'
              } 
         }
         stage('Build green container') {
              steps {
                  sh  '''
                    aws ecr get-login-password --region us-east-1 | docker login --username AWS --password-stdin 164754508673.dkr.ecr.us-east-1.amazonaws.com
                    docker build -t prod .
                    '''
              } 
         }
         stage('Push green container') {
              steps {
                  sh  '''
                     docker tag prod:latest 164754508673.dkr.ecr.us-east-1.amazonaws.com/prod:latest
                     docker push 164754508673.dkr.ecr.us-east-1.amazonaws.com/prod:latest
                     '''
              } 
         }
         stage('Creat K8s config') {
              steps {
                  sh  'aws eks --region us-east-1 update-kubeconfig --name eks-final'
              } 
         }
         stage('Deploy green container') {
              steps {
                  sh  '''
                    if [ kubectl get deployments | grep "blue" ]
                        kubectl delete deployment blue
                    kubectl apply -f ./green-controller.yaml'
              } 
         }
         stage('Redirect service to green container') {
              steps {
                  sh  'kubectl apply -f ./blue-green-service.json'
              } 
         }
         stage('display ILB') {
              steps {
                  sh  'kubectl get services'
              } 
         }
     }
}
