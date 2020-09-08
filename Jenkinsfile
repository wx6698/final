pipeline {
     agent any
     stages {
         stage('AWS Credentials') {
             steps {
                withCredentials([[$class: 'AmazonWebServicesCredentialsBinding', credentialsId: 'bg', ACCESS_KEY: 'ACCESS_KEY', SECRET_KEY: 'SECRET_KEY']]){
                 sh '''
                     mkdir -p ~/.aws
                     echo "[default]" > ~/.aws/credentials
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.aws/credentials
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.aws/credentials
                     cat ~/.aws/credentials
                 '''
                }
             }
         }
         stage('Lint HTML') {
              steps {
                  sh 'aws s3 cp s3://udacity-final/* . --recursive '
              }
         }
     }
}
