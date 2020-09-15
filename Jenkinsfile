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
                     # Validate Cloudformation templates.
                     echo "Validating CloudFormation templates..."
                     for template in cloudformation/*.yml; do
                     yamllint --strict $template
                     cfn-lint --include-checks I --template $template
                     # validate-template requires valid AWS Credentials :(
                     # aws cloudformation validate-template --template-body file://$template --region us-east-1
                     done
                     # Check ansible playbooks for correctness.
                     playbooks="main.yml deploy.yml delete.yml"
                     echo "Checking playbook syntax..."
                     ansible-playbook -i inventory $playbooks --syntax-check
                     echo "Linting playbooks with ansible-lint..."
                     ansible-lint $playbooks
                     '''
              }
         }
         stage('Build blue container') {
              steps {
                  sh  '''
                    aws ecr get-login-password --region us-east-1 | docker login --username AWS --password-stdin 164754508673.dkr.ecr.us-east-1.amazonaws.com
                    docker build -t test .
                    '''
              } 
         }
         stage('Push blue container') {
              steps {
                  sh  '''
                     docker tag test:latest 164754508673.dkr.ecr.us-east-1.amazonaws.com/test:latest
                     docker push 164754508673.dkr.ecr.us-east-1.amazonaws.com/test:latest
                     '''
              } 
         }
         stage('Creat K8s config') {
              steps {
                  sh  'aws eks --region us-east-1 update-kubeconfig --name eks-final'
              } 
         }
         stage('Deploy blue container') {
              steps {
                  sh  'kubectl apply -f ./blue-controller.json'
              } 
         }
         stage('Redirect service to blue container') {
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
